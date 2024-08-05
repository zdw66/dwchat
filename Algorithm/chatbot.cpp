#include "chatbot.h"

ChatBOT::ChatBOT(const QString& groupId, const QString& apiKey, QObject* parent)
    : QObject(parent),groupId(groupId), apiKey(apiKey) {
    manager = new QNetworkAccessManager();
}

void ChatBOT::sendMessage(const QString& messageText) {

    payload["model"] = "abab6.5-chat";
    payload["tokens_to_generate"] = 1024;

    QJsonObject replyConstraints;
    replyConstraints["sender_type"] = "BOT";
    replyConstraints["sender_name"] = "智能助理";
    payload["reply_constraints"] = replyConstraints;

    QJsonObject newUserMessage;
    newUserMessage["sender_type"] = "USER";
    newUserMessage["sender_name"] = "小明";
    newUserMessage["text"] = messageText;
    messages.append(newUserMessage);
    payload["messages"] = messages;

    QJsonArray botSetting;
    QJsonObject botInfo;
    botInfo["bot_name"] = "智能助理";
    botInfo["content"] = "智能聊天给您的生活带来便利！";
    botSetting.append(botInfo);
    payload["bot_setting"] = botSetting;

    QNetworkRequest request(QUrl(apiUrl + groupId));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", ("Bearer " + apiKey).toUtf8());

    QNetworkReply* reply = manager->post(request, QJsonDocument(payload).toJson());
    connect(reply, &QNetworkReply::finished, this, [=]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray responseBytes = reply->readAll();
            QJsonDocument jsonResponse = QJsonDocument::fromJson(responseBytes);
            replyText = jsonResponse["reply"].toString();
            //将当次的ai回复内容加入messages,这样才能在多轮的对话中记住对话历史。
            messages.append(jsonResponse["choices"][0]["messages"][0].toObject());
            payload["messages"] = messages;

            // 使用 cmark 转换 Markdown 到 HTML
            char* html = cmark_markdown_to_html(replyText.toUtf8(), strlen(replyText.toUtf8()), CMARK_OPT_DEFAULT);
            // 使用 CSS 进行样式设置
            htmlText = "<br/>";
            htmlText += QString::fromUtf8(html);
            htmlText.prepend("<style>"
                               "body { font-family: Arial, sans-serif; }"
                               "pre { background: #f0f0f0; padding: 10px; border-radius: 5px; }"
                               "code { background: #f0f0f0; padding: 2px 4px; border-radius: 4px; }"
                               "</style>");
            // 释放内存
            free(html);
            emit ResultOk();
        } else {
            qDebug() << "Error:" << reply->errorString();
        }
        reply->deleteLater();
    });
}
