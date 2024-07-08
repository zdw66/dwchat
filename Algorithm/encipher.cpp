#include "encipher.h"

Encipher::Encipher()
{

}

QByteArray Encipher::generateSalt()
{
    const int saltLength = 16; // 盐的长度
    QByteArray salt(saltLength, 0);
    for (int i = 0; i < saltLength; ++i) {
        salt[i] = qrand() % 256; // 生成随机的盐值
    }
    return salt;
}

QByteArray Encipher::hashPassword(const QString &password, const QByteArray &salt)
{
    const int iterations = 10000; // 迭代次数
    const int keyLength = 64; // 密钥长度
    const QByteArray key = QMessageAuthenticationCode::hash(password.toUtf8(), salt, QCryptographicHash::Sha256);

    QByteArray hashedPassword = key;
    for (int i = 0; i < iterations; ++i) {
        hashedPassword = QMessageAuthenticationCode::hash(hashedPassword, salt + key, QCryptographicHash::Sha256);
    }

    return hashedPassword.toHex();
}

bool Encipher::verifyPassword(const QString &password, const QByteArray &salt, const QByteArray &hashedPassword)
{
    QByteArray calculatedHash = hashPassword(password, salt);
    return (calculatedHash == hashedPassword);
}

