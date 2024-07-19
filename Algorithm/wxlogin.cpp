#include "wxlogin.h"

WxLogin::WxLogin()
{

}

QString WxLogin::getUuId(){
    QTcpSocket socket;
    socket.connectToHost("150.158.50.9", 5555);
    QString UuId;
    while(true){
        if(socket.waitForConnected()) {
            qDebug() << "Connected to server";
            socket.waitForReadyRead();
            QString response = QString::fromLatin1(socket.readAll());
            UuId = response;
            socket.close();
            break;
        } else {
            qDebug() << "Failed to connect to server";
            socket.connectToHost("150.158.50.9", 5555);
        }
    }
    return UuId;
}
