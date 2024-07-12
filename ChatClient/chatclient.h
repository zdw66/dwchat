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
#include <QDir>
#include <QMessageBox>
#include <QDesktopServices>
#include <QPixmap>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <QVBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QBuffer>
#include <QUdpSocket>
#include "algorithm.h"

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
    Emijo *MyEmijo;

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

private slots:
    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

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
private:
    /*video*/
    QUdpSocket *udpSocket;
    QUdpSocket *ClientudpSocket;
    int is_videoShow;
    QWidget *videoShow;
    cv::VideoCapture *capture;
    QPixmap pixmap;
    QLabel label;
    QLabel label2;
    /*file view*/
    QVector<QPair<QNChatMessage*,QString>>toServerFileViewList;
    QVector<QPair<QNChatMessage*,QString>>getServerFileViewList;
    QString toServerFileSize;
    QString getServerFileSize;
};
#endif // CHATCLIENT_H
