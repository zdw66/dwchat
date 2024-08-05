#ifndef CHATBOT_H
#define CHATBOT_H

#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <cmark.h>
#include <QInputDialog>
#include <QSslSocket>
#include <QUrl>
#include <QDebug>

class ChatBOT :public QObject 
{
    Q_OBJECT
public:
    ChatBOT(const QString& groupId, const QString& apiKey, QObject* parent = nullptr);
    void sendMessage(const QString& messageText);
    //ai回复原文
    QString replyText;
    //ai回复转为HTML形式文本
    QString htmlText;
signals:
    void ResultOk();
private:
    QString groupId;
    QString apiKey;
    QNetworkAccessManager* manager;
    QJsonObject payload;
    QJsonArray messages;
    QString apiUrl = "http://api.minimax.chat/v1/text/chatcompletion_pro?GroupId=";
};

#endif // CHATBOT_H
