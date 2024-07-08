#ifndef ENCIPHER_H
#define ENCIPHER_H

#include <QCryptographicHash>
#include <QDebug>
#include <QByteArray>
#include <QString>
#include <QMessageAuthenticationCode>
#include <iostream>
using namespace std;

class Encipher
{
public:
    Encipher();
    QByteArray generateSalt();
    QByteArray hashPassword(const QString &, const QByteArray &);
    bool verifyPassword(const QString &, const QByteArray &, const QByteArray &);

};

#endif // ENCIPHER_H
