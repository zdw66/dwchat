#ifndef AUTH_H
#define AUTH_H

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QDebug>
#include <QEventLoop>
#include <QTextCodec>

class Auth
{
public:
    Auth();
    bool addersJudgment(QString);
private:
    QVector<QString>Route;
};

#endif // AUTH_H
