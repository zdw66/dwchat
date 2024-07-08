#include "chatclient.h"
#include "ui_chatclient.h"

ChatClient::ChatClient(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChatClient)
{
    ui->setupUi(this);
//    AuthCode *s = new AuthCode();
//    s->getEmail();
    myFile = new MyFileInfo(this);
    ui->progressBar->setValue(0);
    m_downloadPath = QCoreApplication::applicationDirPath() + "/../下载";
    isDownloading = false;
    is_videoShow=false;
    udpSocket = new QUdpSocket(this);
    ClientudpSocket = new QUdpSocket(this);
    videoShow = new QWidget();
    videoShow->setWindowTitle("Client");
    if (!ClientudpSocket->bind(QHostAddress::Any,7777)) {
        qDebug() << "error:Client failed to bind to port!";
        return;
    }else qDebug()<<"Client video bind succeed!";
    QDir dir;
    if(!dir.exists(m_downloadPath)) {
        dir.mkdir(m_downloadPath);
    }
    ui->listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    connectToServer();
    connectSigSlots();
}

ChatClient::~ChatClient()
{
    delete ui;
}

void ChatClient::connectSigSlots()
{
    connect(ui->pushButton_3, &QPushButton::clicked, this, [=]() {
        QString path = QCoreApplication::applicationDirPath()+"/../下载";
        QDesktopServices::openUrl(QUrl("file:"+path, QUrl::TolerantMode));
    });
    connect(ui->pushButton,&QPushButton::clicked,this,[=](){
        if(socket->state() != QAbstractSocket::ConnectedState) {
            if(!connectToServer(socket)) {
                return;
            }
        }
        QString messages = ui->textEdit->toPlainText();
        QString time = QString::number(QDateTime::currentDateTime().toTime_t()); //时间戳
        QNChatMessage* messageW = new QNChatMessage(ui->listWidget->parentWidget());
        QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
        dealMessage(messageW, item, messages , time, QNChatMessage::User_Me);
        messageW->setTextSuccess();
        ui->textEdit->setText("");
        qDebug()<<(tr("发送消息：%1").arg(messages));
        QByteArray data=(QString(R"({
                                 "MessageType":2,
                                 "sender":"root2",
                                 "user":"root1",
                                 "MessagesInfo":"%1"
                                 })").arg(messages).toUtf8());
        socket->write(data);
    });
    connect(ui->pushButton_2,&QPushButton::clicked,[=](){
        ServerFilePath = QFileDialog::getOpenFileName(this, "选择文件", "/");
        if(ServerFilePath=="") return;
        QString time = QString::number(QDateTime::currentDateTime().toTime_t()); //时间戳
        QNChatMessage* messageW = new QNChatMessage(ui->listWidget->parentWidget());
        QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
        if(ServerFilePath.endsWith(".jpg")
         ||ServerFilePath.endsWith(".png")
         ||ServerFilePath.endsWith(".gif"))is_images=true;
        else is_images=false;
        if(!is_images)dealMessage(messageW, item, ServerFilePath.right(ServerFilePath.size()-ServerFilePath.lastIndexOf('/')-1), time, QNChatMessage::User_Me);
        else dealimage(messageW, item, ServerFilePath, time, QNChatMessage::User_Me);
        messageW->setTextSuccess();
        QFile file(ServerFilePath);
        QString msg;
        if(file.size()>1024*1024) {
            toServerFileSize = QString("%1M").arg(file.size()/1024/1024.0);
        }
        else {
            toServerFileSize = QString("%1KB").arg(file.size()/1024.0);
        }

        toServerFileViewList.push_back({messageW,ServerFilePath});
        auto i = toServerFileViewList[toServerFileViewList.size()-1];
        i.first->fileSize=toServerFileSize;

        connect(i.first,&QNChatMessage::clicked,[=](){
            if(!i.second.endsWith(".jpg")
             &&!i.second.endsWith(".png")
             &&!i.second.endsWith(".gif")){
                if(!i.second.endsWith(".zip")
                 &&!i.second.endsWith(".rar")
                 &&!i.second.endsWith(".tar.gz")){
                    // 读取文件内容
                    QFile file(i.second);
                    file.open(QIODevice::ReadOnly | QIODevice::Text);
                    QTextStream in(&file);
                    QString fileContent = in.readAll();
                    QTextEdit *textEdit = new QTextEdit();
                    textEdit->setText(fileContent);
                    textEdit->resize(600,600);
                    textEdit->setWindowTitle(i.second.right(i.second.size()-i.second.lastIndexOf('/')-1));
                    textEdit->show();
                }else{
                    QString path = QCoreApplication::applicationDirPath()+"/../下载";
                    QDesktopServices::openUrl(QUrl("file:"+path, QUrl::TolerantMode));
                }
            }
        });

        dealimage(i.first, item, ServerFilePath, time, QNChatMessage::User_Me);
        i.first->setTextSuccess();
        is_images=false;

        ToServerFileInfo(socket);
    });
}
/*ServeR Init*/
void ChatClient::connectToServer()
{
    socket = new QTcpSocket(this);
    connectToServer(socket);
    connect(socket, &QTcpSocket::readyRead, this, [=](){
        readServerMsg();
    });
    connect(socket, &QTcpSocket::disconnected, this, [=]() {
        qDebug()<<(QString("与服务器断开连接：原因：%1").arg(socket->errorString()));
    });
    connect(ClientudpSocket, &QUdpSocket::readyRead, this, [=]() {
        while (ClientudpSocket->hasPendingDatagrams()) {
            QByteArray datagram;
            datagram.resize(ClientudpSocket->pendingDatagramSize());
            ClientudpSocket->readDatagram(datagram.data(), datagram.size());
            if(datagram=="Commopasd"||datagram==""){
                if(datagram=="Commopasd"){
                    label.clear();
                    videoShow->hide();
                }
                is_videoShow=false;
                return;
            }
            QImage image;
            image.loadFromData(datagram, "JPEG");
            QPixmap receivedPixmap;
            receivedPixmap = QPixmap::fromImage(image.rgbSwapped());

            label.setPixmap(receivedPixmap);
             // 处理接收到的QPixmap，例如显示在界面上或保存到文件中
        }//
        label.setScaledContents(true);
        QVBoxLayout layout;
        layout.addWidget(&label);
        label.resize(QSize(800,600));
        videoShow->resize(QSize(800,600));
        videoShow->setLayout(&layout);
        if(!is_videoShow){
            videoShow->show();
        }
        is_videoShow=true;

    });
}

bool ChatClient::connectToServer(QTcpSocket *socket)
{
    socket->connectToHost("192.168.12.129",1234);

    if(!socket->waitForConnected(2*1000)) {
        qDebug()<<"error!";
        return false;
    }
    QByteArray data=(QString(R"({
                             "MessageType":3,
                             "sender":"root2",
                             "user":"root1",
                             "MessagesInfo":""
                             })")).toUtf8();
    socket->write(data);
    qDebug()<<"OK!";
    return true;
}

void ChatClient::readServerMsg()
{
    if(isDownloading) {//ReadServerFileData
        fileDataRead();
        return;
    }
    /* if not is FlieData*/
    qDebug()<< ".............ReadServerMsg................";

    QString data = socket->readAll();
    QJsonDocument jsonDocument = QJsonDocument::fromJson(data.toUtf8());
    QVariant variant = jsonDocument.toVariant();
    QMap<QString, QVariant> map = variant.toMap();//JSON DATA TO C++ DATA

    int typeMsg = map["MessageType"].toInt();

    if(typeMsg == 0) {//ReadServerFileInfo
        fileInfoRead(map);
        isDownloading = true;
    }else if(typeMsg==1){
        ToServerFileData(socket);
    }
    else if(typeMsg==2){//ReadServerMessages
        QString time = QString::number(QDateTime::currentDateTime().toTime_t());
        dealMessageTime(time);
        QString Messages = map["MessagesInfo"].toString();
        QNChatMessage* messageW = new QNChatMessage(ui->listWidget->parentWidget());
        QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
        dealMessage(messageW, item, Messages, time, QNChatMessage::User_She);
    }
}
/*File Read*/
void ChatClient::fileInfoRead(QMap<QString, QVariant> map)
{
    qDebug()<<"文件信息读取on_fileInfoRead......";
    // 接收文件大小，数据总大小信息和文件名大小,文件名信息
    myFile->fileName = map["FileName"].toString();
    myFile->fileSize = map["FileSize"].toInt();
    // 获取文件名，建立文件
    qDebug()<<(QString("Client下载文件 %1, 文件大小：%2").arg(myFile->fileName).arg(myFile->fileSize));
    filePath = m_downloadPath + "/" + myFile->fileName;
    myFile->localFile.setFileName(filePath);
    if(myFile->fileSize>1024*1024) {
        getServerFileSize = QString("%1M").arg(myFile->fileSize/1024/1024.0);
    }
    else {
        getServerFileSize = QString("%1KB").arg(myFile->fileSize/1024.0);
    }

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
    socket->write(senddata);
}

void ChatClient::fileDataRead()
{
    qint64 readBytes = socket->bytesAvailable();
    if(readBytes <0) return;

    int progress = 0;
    // 如果接收的数据大小小于要接收的文件大小，那么继续写入文件
    if(myFile->bytesReceived < myFile->fileSize) {
        // 返回等待读取的传入字节数
        QByteArray data = socket->read(readBytes);
        myFile->bytesReceived+=data.size();
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

        getServerFileViewList.push_back({new QNChatMessage(ui->listWidget->parentWidget()),filePath});
        getServerFileViewList.back().first->fileSize=getServerFileSize;
        auto i = getServerFileViewList[getServerFileViewList.size()-1];
        connect(i.first,&QNChatMessage::clicked,[=](){
            if(!i.second.endsWith(".jpg")
             &&!i.second.endsWith(".png")
             &&!i.second.endsWith(".gif")){
                if(!i.second.endsWith(".zip")
                 &&!i.second.endsWith(".rar")
                 &&!i.second.endsWith(".tar.gz")){
                    // 读取文件内容
                    QFile file(i.second);
                    file.open(QIODevice::ReadOnly | QIODevice::Text);
                    QTextStream in(&file);
                    QString fileContent = in.readAll();
                    QTextEdit *textEdit = new QTextEdit();
                    textEdit->setText(fileContent);
                    textEdit->resize(600,600);
                    textEdit->setWindowTitle(i.second.right(i.second.size()-i.second.lastIndexOf('/')-1));
                    textEdit->show();
                }else{
                    QString path = QCoreApplication::applicationDirPath()+"/../下载";
                    QDesktopServices::openUrl(QUrl("file:"+path, QUrl::TolerantMode));
                }
            }
        });
        QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
        is_images=true;
        dealimage(getServerFileViewList.back().first, item, filePath, time, QNChatMessage::User_She);
        getServerFileViewList.back().first->setTextSuccess();
        is_images=false;
    }

    if (myFile->bytesReceived > myFile->fileSize){
        qDebug()<<"myFile->bytesReceived > m_fileSize";
    }
}
/*File To Server*/
void ChatClient::ToServerFileInfo(QTcpSocket *socket)
{
    //获取文件数据，准备发送
    QPair<QString,qint64>  DataInfoBlock = getFileContent(ServerFilePath);
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

QPair<QString,qint64> ChatClient::getFileContent(QString filePath)
{
    if(!QFile::exists(filePath)) {
        qDebug()<<(QString("没有要传输的文件！" + filePath));
        return {};
    }
    m_sendFilePath = filePath;
    qDebug()<<(QString("正在获取文件信息[%1]......").arg(filePath));
    QFileInfo info(filePath);

    //获取要发送的文件大小
    m_sendFileSize = info.size();

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

void ChatClient::ToServerFileData(QTcpSocket *socket)
{

    qint64 payloadSize = 1024*1; //每一帧发送1024*64个字节，控制每次读取文件的大小，从而传输速度

    double progressByte= 0;//发送进度
    qint64 bytesWritten=0;//已经发送的字节数

    while(bytesWritten != m_sendFileSize) {
        double temp = bytesWritten/1.0/m_sendFileSize*100;
        int  progress = static_cast<int>(bytesWritten/1.0/m_sendFileSize*100);
        if(bytesWritten<m_sendFileSize){
            QByteArray DataInfoBlock = m_localFile.read(qMin(m_sendFileSize,payloadSize));
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
            qDebug()<<(QString("当前上传进度：%1/%2 -> %3%").arg(bytesWritten).arg(m_sendFileSize).arg(progress));
            progressByte+=0.1;
        }
    }
}

/*气泡式聊天框*/
void ChatClient::dealMessage(QNChatMessage *messageW, QListWidgetItem *item, QString text, QString time,  QNChatMessage::User_Type type)
{
    messageW->setFixedWidth(this->width());
    QSize size = messageW->fontRect(text,false);
    item->setSizeHint(size);
    messageW->setText(text, time, size, type,false);
    ui->listWidget->setItemWidget(item, messageW);
}
void ChatClient::dealimage(QNChatMessage *messageW, QListWidgetItem *item, QString filePath, QString time,  QNChatMessage::User_Type type)
{
    messageW->setFixedWidth(this->width());
    QImage image(filePath);
    QSize size = messageW->fontRect(filePath,true);
    item->setSizeHint(size);
    messageW->setText(filePath, time, size, type,true);
    ui->listWidget->setItemWidget(item, messageW);
}

void ChatClient::dealMessageTime(QString curMsgTime)
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

void ChatClient::resizeEvent(QResizeEvent *event)
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

void ChatClient::on_pushButton_4_clicked()
{
    capture = new cv::VideoCapture();
    // 尝试打开默认摄像头（索引为0）
    if (!capture->open(0)) {
        qDebug() << "Error opening video capture";
        return;
    }

    // 设置定时器以定期从摄像头捕获帧
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=](){
        cv::Mat frame;
        *capture >> frame; // 从摄像头捕获帧

        QImage qtImage;
        if (!frame.empty()) {
            cvtColor(frame, frame, cv::COLOR_BGR2RGB); // 转换为RGB
            qtImage = QImage((const unsigned char*)(frame.data), frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
            qtImage = QImage(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888).rgbSwapped();
        }
        QPixmap pixmap = QPixmap::fromImage(qtImage.rgbSwapped());
        label2.clear();
        label2.setPixmap(pixmap); // OpenCV使用BGR，而Qt使用RGB
        // 将QPixmap转换为字节数组
        QBuffer buffer;
        buffer.open(QIODevice::WriteOnly);
        qtImage.save(&buffer, "JPEG");

        // 发送QPixmap数据到服务端
        udpSocket->writeDatagram(buffer.data(), buffer.size(), QHostAddress("192.168.12.129"), 7755);

    });
    label2.setScaledContents(true);
    QVBoxLayout layout;
    layout.addWidget(&label2);
    label2.resize(QSize(800,600));
    videoShow->setLayout(&layout);
    videoShow->resize(QSize(800,600));
    videoShow->show();
    timer->start(30); // 每30毫秒捕获一次帧
}

void ChatClient::on_pushButton_5_clicked()
{
    udpSocket->writeDatagram("Commopasd", QHostAddress("192.168.12.129"),7755);
    capture->release();
    videoShow->hide();
}
