#include "chatserver.h"
#include "ui_chatserver.h"

ChatServer::ChatServer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChatServer)
{
    ui->setupUi(this);
    myFile = new MyFileInfo(this);
    ui->progressBar->setValue(0);
    m_downloadPath = QCoreApplication::applicationDirPath() + "/../下载";
    isDownloading = false;
    QDir dir;
    if(!dir.exists(m_downloadPath)) {
        dir.mkdir(m_downloadPath);
    }
    this->server=new QTcpServer(this);
    startServer();
    ui->listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    server->setMaxPendingConnections(50);
    connectSigSlots();
}

ChatServer::~ChatServer()
{
    delete ui;
}

void ChatServer::connectSigSlots()
{
    connect(ui->pushButton_2, &QPushButton::clicked, [=]() {
        filePath = QFileDialog::getOpenFileName(this, "选择文件", "./",("*.jpg *.png *.gif"));
        if(filePath=="") return;
        QString time = QString::number(QDateTime::currentDateTime().toTime_t()); //时间戳
        QNChatMessage* messageW = new QNChatMessage(ui->listWidget->parentWidget());
        QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
        if(filePath.endsWith(".jpg")
         ||filePath.endsWith(".png")
         ||filePath.endsWith(".gif"))is_images=true;
        else is_images=false;
        if(!is_images)dealMessage(messageW, item, filePath.right(filePath.size()-filePath.lastIndexOf('/')-1), time, QNChatMessage::User_Me);
        else dealimage(messageW, item, filePath, time, QNChatMessage::User_Me);
        messageW->setTextSuccess();
        QFile file(filePath);
        QString msg;
        if(file.size()>1024*1024) {
            msg = QString("大小：%1M").arg(file.size()/1024/1024.0);
        }
        else {
            msg = QString("大小：%1KB").arg(file.size()/1024.0);
        }
        ToClientFileInfo(arrayClient["root2"]);
    });
}


bool ChatServer::startServer()
{
    qDebug()<<(QString("服务器已启动，监听端口：%1").arg(1234));

    server->listen(QHostAddress::Any, 1234);
    connect(server, &QTcpServer::newConnection, this,[=](){
        QTcpSocket* socket = server->nextPendingConnection();
        qDebug()<<(QString("客户端[%1]已连接！").arg(socket->peerAddress().toString()));
        connect(socket, &QTcpSocket::readyRead, [=]() {
            dealMsg(socket);
        });
    });
    return true;
}

void ChatServer::dealMsg(QTcpSocket *socket)
{
    if(isDownloading) {//filedata download
        fileDataRead();
        return;
    }
    qDebug()<< ".............ReadClientMsg................";

    QString data = socket->readAll();
    QJsonDocument jsonDocument = QJsonDocument::fromJson(data.toUtf8());
    QVariant variant = jsonDocument.toVariant();
    QMap<QString, QVariant> map = variant.toMap();
    int typeMsg = map["MessageType"].toInt();
    if(typeMsg == 0) {//fileinfo
        fileInfoRead(map);
        isDownloading = true;
    }
    else if(typeMsg == 1) {
        // 发送文件数据
        ToClientFileData(socket);
    }else if(typeMsg == 2){
        QString time = QString::number(QDateTime::currentDateTime().toTime_t()); //时间戳
        dealMessageTime(time);
        QString Messages = map["MessagesInfo"].toString();
        QNChatMessage* messageW = new QNChatMessage(ui->listWidget->parentWidget());
        QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
        dealMessage(messageW, item, Messages, time, QNChatMessage::User_She);
    }else if(typeMsg==3){
        QString sender = map["sender"].toString();
        if(arrayClient[sender]==nullptr)arrayClient[sender]=socket;
    }
}

void ChatServer::fileDataRead()
{
    qint64 readBytes = arrayClient["root2"]->bytesAvailable();
    if(readBytes <0) return;

    int progress = 0;
    // 如果接收的数据大小小于要接收的文件大小，那么继续写入文件
    if(myFile->bytesReceived < myFile->fileSize) {
        // 返回等待读取的传入字节数
        QByteArray data = arrayClient["root2"]->read(readBytes);
        myFile->bytesReceived+=readBytes;
        qDebug()<<(QString("接收进度：%1/%2(字节)").arg(myFile->bytesReceived).arg(myFile->fileSize));
        progress =static_cast<int>(myFile->bytesReceived*100/myFile->fileSize);
        myFile->progressByte = myFile->bytesReceived;
        myFile->progressStr = QString("%1").arg(progress);
        ui->progressBar->setValue(progress);
        myFile->localFile.write(data);
    }

    // 接收数据完成时
    if (myFile->bytesReceived==myFile->fileSize){
        qDebug()<<(tr("接收文件[%1]成功！").arg(myFile->fileName));
        progress = 100;
        myFile->localFile.close();
        qDebug()<<(QString("接收进度：%1/%2（字节）").arg(myFile->bytesReceived).arg(myFile->fileSize));
        myFile->progressByte = myFile->bytesReceived;
        ui->progressBar->setValue(progress);
        isDownloading = false;
        myFile->initReadData();
        QString time = QString::number(QDateTime::currentDateTime().toTime_t()); //时间戳
        QNChatMessage* messageW = new QNChatMessage(ui->listWidget->parentWidget());
        QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
        if(filePath.endsWith(".jpg")
         ||filePath.endsWith(".png")
         ||filePath.endsWith(".gif"))is_images=true;
        else is_images=false;
        if(!is_images)dealMessage(messageW, item, filePath.right(filePath.size()-filePath.lastIndexOf('/')-1), time, QNChatMessage::User_Me);
        else dealimage(messageW, item, filePath, time, QNChatMessage::User_She);
        messageW->setTextSuccess();
    }

    if (myFile->bytesReceived > myFile->fileSize){
        qDebug()<<"myFile->bytesReceived > m_fileSize";
    }
}

void ChatServer::fileInfoRead(QMap<QString, QVariant> map)
{
    qDebug()<<"文件信息读取on_fileInfoRead......";
    // 接收文件大小，数据总大小信息和文件名大小,文件名信息
    myFile->fileName = map["FileName"].toString();
    myFile->fileSize = map["FileSize"].toInt();
    // 获取文件名，建立文件
    qDebug()<<(QString("Server下载文件 %1, 文件大小：%2").arg(myFile->fileName).arg(myFile->fileSize));
    filePath = m_downloadPath + "/" + myFile->fileName;
    myFile->localFile.setFileName(filePath);
    // 打开文件，准备写入
    if(!myFile->localFile.open(QIODevice::WriteOnly)) {
        qDebug()<<"文件打开失败！";
    }
    //文件信息获取完成，接着获取文件数据
    QByteArray senddata=(QString(R"({
                             "MessageType":1,
                             "sender":"root2",
                             "user":"root1",
                             "MessagesInfo":""
                             })")).toUtf8();
    arrayClient["root2"]->write(senddata);
}

void ChatServer::ToClientFileInfo(QTcpSocket *socket)
{
    //获取文件数据，准备发送
    QPair<QString,qint64>  DataInfoBlock = getFileContent(filePath);
    QByteArray data=(QString(R"({
                             "MessageType":0,
                             "sender":"root2",
                             "user":"root1",
                             "MessagesInfo":"",
                             "FileName":"%1",
                             "FileSize":%2
                             })").arg(DataInfoBlock.first).arg(DataInfoBlock.second)).toUtf8();
    QThread::msleep(10); //添加延时
    m_fileInfoWriteBytes = socket->write(data) - typeMsgSize;
    qDebug()<< "传输文件信息，大小："<< m_sendFileSize;
    //等待发送完成，才能继续下次发送，否则发送过快，对方无法接收
    if(!socket->waitForBytesWritten(10*1000)) {
        qDebug()<<(QString("网络请求超时,原因：%1").arg(socket->errorString()));
        return;
    }

    qDebug()<<(QString("文件信息发送完成，开始对[%1]进行文件传输------------------")
                        .arg(socket->localAddress().toString()));
    qDebug()<<"当前文件传输线程id:"<<QThread::currentThreadId();

    m_localFile.setFileName(m_sendFilePath);
    if(!m_localFile.open(QFile::ReadOnly)){
        qDebug()<<(QString("文件[%1]打开失败！").arg(m_sendFilePath));
        return;
    }
}

void ChatServer::ToClientFileData(QTcpSocket *socket)
{

    qint64 payloadSize = 1024*1; //每一帧发送1024*64个字节，控制每次读取文件的大小，从而传输速度

    double progressByte= 0;//发送进度
    qint64 bytesWritten=0;//已经发送的字节数

    while(bytesWritten != m_sendFileSize) {
        double temp = bytesWritten/1.0/m_sendFileSize*100;
        int  progress = static_cast<int>(bytesWritten/1.0/m_sendFileSize*100);
        if(bytesWritten<m_sendFileSize){
            QByteArray DataInfoBlock = m_localFile.read(qMin(m_sendFileSize,payloadSize));
//            DataInfoBlock = encryptImage(DataInfoBlock,"qudnakcospxksjcla");
            qint64 WriteBolockSize = socket->write(DataInfoBlock, DataInfoBlock.size());
            QThread::usleep(3); //添加延时，防止服务端发送文件帧过快，否则发送过快，客户端接收不过来，导致丢包
            //等待发送完成，才能继续下次发送，
            if(!socket->waitForBytesWritten(3*1000)) {
                qDebug()<<("网络请求超时");
                return;
            }
            bytesWritten += WriteBolockSize;
        }

        if(bytesWritten==m_sendFileSize){
            //LogWrite::LOG_DEBUG(QString("当前更新进度：100%,发送总次数:%1").arg(count), "server_"+socket->localAddress().toString());
            qDebug()<<(QString("当前上传进度：%1/%2 -> %3%").arg(bytesWritten).arg(m_sendFileSize).arg(progress));
            qDebug()<<(QString("-------------对[%1]的文件传输完成！------------------").arg(socket->peerAddress().toString()));

            m_localFile.close();
            return;
        }
        if(bytesWritten > m_sendFileSize) {
            qDebug()<<("意外情况！！！");
            return;
        }

        if(bytesWritten/1.0/m_sendFileSize > progressByte) {
            qDebug()<<(QString("当前上传进度：%1/%2 -> %3%").arg(bytesWritten).arg(jiamiFileSize).arg(progress));
            progressByte+=0.1;
        }

    }

}

QPair<QString,qint64> ChatServer::getFileContent(QString filePath)
{
    if(!QFile::exists(filePath)) {
        qDebug()<<(QString("没有要传输的文件！" + filePath));
        return {};
    }
    m_sendFilePath = filePath;
    qDebug()<<(QString("正在获取文件信息[%1]......").arg(filePath));
    //获取要发送的文件大小
    QFileInfo info(filePath);
    m_sendFileSize=info.size();

    qDebug()<<(QString("要发送的文件大小：%1字节，%2M").arg(m_sendFileSize).arg(m_sendFileSize/1024/1024.0));

    //获取发送的文件名
    QString currentFileName=filePath.right(filePath.size()-filePath.lastIndexOf('/')-1);

    qDebug()<<(QString("文件[%1]信息获取完成！").arg(currentFileName));
    //发送的文件总大小中，信息类型不计入
    QString msg;
    if(m_sendFileSize>1024*1024) {
        msg = QString("%1M").arg(m_sendFileSize/1024/1024.0);
    }
    else {
        msg = QString("%1KB").arg(m_sendFileSize/1024.0);
    }
    qDebug()<<(QString("发送的文件名：%1，文件大小：%2").arg(currentFileName).arg(msg));

    return {currentFileName,m_sendFileSize};
}

bool ChatServer::checkFile(QString filePath)
{
    QFile file(filePath);
    if(!file.exists(filePath)){
        qDebug()<<("上传文件不存在！");
        return false;
    }
    if(file.size()==0) {
        qDebug()<<("上传文件为空文件！");
        return false;
    }
    return true;
}

/*气泡式聊天框*/
void ChatServer::dealMessage(QNChatMessage *messageW, QListWidgetItem *item, QString text, QString time,  QNChatMessage::User_Type type)
{
    messageW->setFixedWidth(this->width());
    QSize size = messageW->fontRect(text,false);
    item->setSizeHint(size);
    messageW->setText(text, time, size, type,false);
    ui->listWidget->setItemWidget(item, messageW);
}
void ChatServer::dealimage(QNChatMessage *messageW, QListWidgetItem *item, QString filePath, QString time,  QNChatMessage::User_Type type)
{
    messageW->setFixedWidth(this->width());
    QImage image(filePath);
    QSize size = messageW->fontRect(filePath,true);
    item->setSizeHint(size);
    messageW->setText(filePath, time, size, type,true);
    ui->listWidget->setItemWidget(item, messageW);
}

void ChatServer::dealMessageTime(QString curMsgTime)
{
    bool isShowTime = false;
    if(ui->listWidget->count() > 0) {
        QListWidgetItem* lastItem = ui->listWidget->item(ui->listWidget->count() - 1);
        QNChatMessage* messageW = (QNChatMessage*)ui->listWidget->itemWidget(lastItem);
        int lastTime = messageW->time().toInt();
        int curTime = curMsgTime.toInt();
        qDebug() << "curTime lastTime:" << curTime - lastTime;
        isShowTime = ((curTime - lastTime) > 60); // 两个消息相差一分钟
//        isShowTime = true;
    } else {
        isShowTime = true;
    }
    if(isShowTime) {
        QNChatMessage* messageTime = new QNChatMessage(ui->listWidget->parentWidget());
        QListWidgetItem* itemTime = new QListWidgetItem(ui->listWidget);
        QSize size = QSize(this->width(), 40);
        messageTime->resize(size);
        itemTime->setSizeHint(size);
        messageTime->setText(curMsgTime, curMsgTime, size, QNChatMessage::User_Time,false);
        ui->listWidget->setItemWidget(itemTime, messageTime);
    }
}

void ChatServer::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);


//    ui->textEdit->resize(this->width() - 160, ui->widget->height() - 10);
//    ui->textEdit->move(10, 10);
//    ui->listWidget->resize(this->width()-20,this->height()-200);
//    ui->pushButton->move(ui->textEdit->width()+ui->textEdit->x() - ui->pushButton->width() - 10,
//                         ui->textEdit->height()+ui->textEdit->y() - ui->pushButton->height() - 10);


    for(int i = 0; i < ui->listWidget->count(); i++) {
        QNChatMessage* messageW = (QNChatMessage*)ui->listWidget->itemWidget(ui->listWidget->item(i));
        QListWidgetItem* item = ui->listWidget->item(i);
        if(messageW->text().endsWith(".jpg")
         ||messageW->text().endsWith(".png")
         ||messageW->text().endsWith(".gif"))is_images=true;
        else is_images=false;
        if(!is_images)dealMessage(messageW, item, messageW->text(), messageW->time(), messageW->userType());
        else dealimage(messageW, item, messageW->text(), messageW->time(), messageW->userType());
    }
}



void ChatServer::on_pushButton_3_clicked()
{
    QString messages = ui->textEdit->toPlainText();
    ui->textEdit->setText("");
    QByteArray data=(QString(R"({
                             "MessageType":2,
                             "sender":"root2",
                             "user":"root1",
                             "MessagesInfo":"%1"
                             })").arg(messages).toUtf8());
    qDebug()<<"arrayClient[\"root2\"]:"<<arrayClient["root2"];
    arrayClient["root2"]->write(data);
    QString time = QString::number(QDateTime::currentDateTime().toTime_t()); //时间戳
    QNChatMessage* messageW = new QNChatMessage(ui->listWidget->parentWidget());
    QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
    dealMessage(messageW, item, messages , time, QNChatMessage::User_Me);
    messageW->setTextSuccess();
}

void ChatServer::on_pushButton_clicked()
{
    QString path = QCoreApplication::applicationDirPath()+"/../下载";
    QDesktopServices::openUrl(QUrl("file:"+path, QUrl::TolerantMode));
}

