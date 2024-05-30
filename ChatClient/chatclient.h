#ifndef CHATCLIENT_H
#define CHATCLIENT_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QThread>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariantMap>
#include <QListWidgetItem>
#include <QPixmap>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <QDir>
#include <QMessageBox>
#include <QDesktopServices>

#include "chatmessage/qnchatmessage.h"
#include "myfileinfo.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ChatClient; }
QT_END_NAMESPACE

class ChatClient : public QWidget
{
    Q_OBJECT

public:
    ChatClient(QWidget *parent = nullptr);
    ~ChatClient();
    QTcpSocket *socket;

private:
    Ui::ChatClient *ui;

private:
    void readServerMsg();
    void fileInfoRead(QMap<QString, QVariant> map);
    void fileDataRead();

private:
    void connectSigSlots();
    void connectToServer();
    bool connectToServer(QTcpSocket *socket);
    /*Server*/
    bool startServer();
    /*Messages Show*/
    void dealMessage(QNChatMessage *messageW, QListWidgetItem *item, QString text, QString time, QNChatMessage::User_Type type);
    void dealMessageTime(QString curMsgTime);
    void dealimage(QNChatMessage *messageW, QListWidgetItem *item, QString filePath, QString time,  QNChatMessage::User_Type type);
protected:
    void resizeEvent(QResizeEvent *event);
    void ToServerFileInfo(QTcpSocket *socket);
    QPair<QString,qint64> getFileContent(QString filePath);
    void ToServerFileData(QTcpSocket *socket);

private:
    MyFileInfo* myFile;
    QString m_downloadPath;//下载路径
    bool isDownloading; //是否正在下载标识
    /*File*/
    QString filePath;
    QString ServerFilePath;
    bool is_images;
    /*Messages*/
    qint64 typeMsgSize;
    qint64 m_totalBytes;
    qint64 m_sendFileSize;
    qint64 m_fileInfoWriteBytes;
    QString m_sendFilePath;
    QFile m_localFile;
};
#endif // CHATCLIENT_H
