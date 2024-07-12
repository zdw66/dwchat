#include "auth.h"

Auth::Auth()
{

}
bool addersJudgment(QString oldAdders){
    QNetworkAccessManager manager;
    QNetworkReply *reply = manager.get(QNetworkRequest(QUrl("http://whois.pconline.com.cn/ipJson.jsp?json=true")));
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    QByteArray data = reply->readAll();
    QTextCodec *codecGBK = QTextCodec::codecForName("GBK");
    // 将 GBK 编码的数据转换为 Unicode 字符串
    QString unicodeString = codecGBK->toUnicode(data);
    QJsonDocument jsonDocument = QJsonDocument::fromJson(unicodeString.toUtf8());
    QJsonObject jsonObject = jsonDocument.object();
    QString ip = jsonObject.value("ip").toString();
    QString address = jsonObject.value("addr").toString();
//    qDebug()<<("ip:"+ip);
//    qDebug()<<("localtion:"+address.split(" ")[0]);
    reply->deleteLater();
    return address.split(" ")[0]==oldAdders;
}
