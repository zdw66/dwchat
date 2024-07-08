#ifndef AUTHCODE_H
#define AUTHCODE_H

#include <QtNetwork>
#include <QTime>
#include <QDebug>
#include <QFile>

class AuthCode
{
public:
    AuthCode(QString ToEmail);
    void generateVerificationCode();
    bool verifyVerificationCode();
    void getEmail();
public:
    QString code;
    qint64 timestamp;
    QString FromEmail;
    QString ToEmail;
};

#endif // AUTHCODE_H
