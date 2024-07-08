#include "authcode.h"

AuthCode::AuthCode(QString ToEmail)
{
    this->ToEmail=ToEmail;
    generateVerificationCode();
}

void AuthCode::generateVerificationCode(){
    const QString possibleCharacters = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    QString code;
    QTime time = QTime::currentTime();
    qsrand(static_cast<uint>(time.msec()));

    for (int i = 0; i < 6; ++i) {
        int index = qrand() % possibleCharacters.length();
        QChar nextChar = possibleCharacters.at(index);
        code.append(nextChar);
    }

    this->code = code;
    this->timestamp = QDateTime::currentMSecsSinceEpoch();

}
bool AuthCode::verifyVerificationCode(){
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    qint64 timeDifference = (currentTime - this->timestamp) / 1000; // Convert to seconds

    if (timeDifference <= 300) { // 5 minutes = 300 seconds
        return true;
    } else {
        return false;
    }
}
void AuthCode::getEmail(){
    QFile file("/home/zdw/QT/DWChat/Algorithm/app.config");
    file.open(QIODevice::ReadOnly);
    QJsonDocument jsonDocument = QJsonDocument::fromJson(file.readAll());
    QVariant variant = jsonDocument.toVariant();
    QMap<QString, QVariant> map = variant.toMap();

    QByteArray EmailAddress = map["EmailConfig"].toMap()["EmailAddress"]
            .toString().toUtf8().toBase64()+"\r\n";
    QByteArray AuthCode = map["EmailConfig"].toMap()["AuthCode"]
            .toString().toUtf8().toBase64()+"\r\n";
    this->FromEmail = map["EmailConfig"].toMap()["EmailAddress"]
            .toString();

    QTcpSocket socket;
    socket.connectToHost("smtp.qq.com", 25); // 使用 QQ 邮箱的 SMTP 服务器
    if(socket.waitForConnected()){
        qDebug() << "Connected to host";
        if(socket.waitForReadyRead()){
            qDebug() << socket.readAll(); // 读取服务器响应信息
        }

        // 发送邮件内容
        QString message = QString("Subject:=?utf-8?B?6aqM6K+B56CB+V?=\r\n"
                          "To: %1\r\n"
                          "From: <%2>\r\n"
                          "Content-Type: text/plain; charset=UTF8;\r\n"
                          "您好，尊敬的用户!欢迎您注册DWChat。\r\n"
                          "请回填如下验证码:%3。\r\n"
                          "验证码在5分钟内有效，5分钟后请重新激活。如有疑问欢迎咨询微信公众号(zdwLOVEwsy)。\r\n"
                          ".\r\n").arg(this->ToEmail).arg(this->FromEmail).arg(this->code);
        socket.write("EHLO smtp.qq.com\r\n");
        if(socket.waitForReadyRead()){
            qDebug() << socket.readAll(); // 读取服务器响应信息
        }
        socket.write("AUTH LOGIN\r\n");
        if(socket.waitForReadyRead()){
            qDebug() << socket.readAll(); // 读取服务器响应信息
        }
        socket.write(EmailAddress); // QQ邮箱账号的base64编码
        if(socket.waitForReadyRead()){
            qDebug() << socket.readAll(); // 读取服务器响应信息
        }
        socket.write(AuthCode); // QQ邮箱密码的base64编码
        if(socket.waitForReadyRead()){
            qDebug() << socket.readAll(); // 读取服务器响应信息
        }
        socket.write("MAIL FROM: <180984282@qq.com>\r\n");
        if(socket.waitForReadyRead()){
            qDebug() << socket.readAll(); // 读取服务器响应信息
        }
        socket.write("RCPT TO: <180984282@qq.com>\r\n");
        if(socket.waitForReadyRead()){
            qDebug() << socket.readAll(); // 读取服务器响应信息
        }
        socket.write("DATA\r\n");
        if(socket.waitForReadyRead()){
            qDebug() << socket.readAll(); // 读取服务器响应信息
        }
        socket.write(message.toUtf8());
        if(socket.waitForReadyRead()){
            qDebug() << socket.readAll(); // 读取服务器响应信息
        }
        socket.write("QUIT\r\n");
    } else {
        qDebug() << "Failed to connect to host";
    }
    socket.close();
}
