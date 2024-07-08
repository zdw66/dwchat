#ifndef CHATSERVER_H
#define CHATSERVER_H

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
#include <QDesktopServices>
#include <QPixmap>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <QVBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QBuffer>
#include <QUdpSocket>

#include "chatmessage/qnchatmessage.h"
#include "chatmessage/myfileinfo.h"
#include "algorithm.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ChatServer; }
QT_END_NAMESPACE

class ChatServer : public QWidget
{
    Q_OBJECT

public:
    ChatServer(QWidget *parent = nullptr);
    ~ChatServer();

private:
    Ui::ChatServer *ui;
    /*Server*/
    void connectSigSlots();
    bool startServer();
    /*Messages Show*/
    void dealMessage(QNChatMessage *messageW, QListWidgetItem *item, QString text, QString time, QNChatMessage::User_Type type);
    void dealMessageTime(QString curMsgTime);
    void dealimage(QNChatMessage *messageW, QListWidgetItem *item, QString filePath, QString time,  QNChatMessage::User_Type type);
protected:
    void resizeEvent(QResizeEvent *event);
private:
    /*File Out*/
    void ToClientFileData(QTcpSocket* socket);
    void ToClientFileInfo(QTcpSocket* socket);
    QPair<QString,qint64> getFileContent(QString filePath);
    bool checkFile(QString filePath);
    void fileDataRead();
    void fileInfoRead(QMap<QString, QVariant> map);

public slots:
    void dealMsg(QTcpSocket* socket);

private slots:
    void on_pushButton_3_clicked();

    void on_pushButton_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

private:
    /*Server*/
    QTcpServer *server;
    QMap<QString,QTcpSocket*>arrayClient;
    /*File*/
    QString filePath;
    bool is_images;
    /*Messages*/
    qint64 typeMsgSize;
    qint64 m_totalBytes;
    qint64 m_sendFileSize;
    qint64 jiamiFileSize;
    qint64 m_fileInfoWriteBytes;
    QString m_sendFilePath;
    QFile m_localFile;
    bool isDownloading;
    MyFileInfo* myFile;
    QString m_downloadPath;//下载路径

private:
    QUdpSocket *udpSocket;
    QUdpSocket *ServerudpSocket;
    cv::VideoCapture *capture;
    QWidget *videoShow;
    QLabel label;
    QLabel label2;
    bool is_videoShow;
    QPixmap pixmap;
    /*file view*/
    QVector<QPair<QNChatMessage*,QString>>toClientFileViewList;
    QVector<QPair<QNChatMessage*,QString>>getClientFileViewList;
    QString toClientFileSize;
    QString getClientFileSize;
};
#endif // CHATSERVER_H
