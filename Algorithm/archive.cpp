#include "archive.h"

Archive::Archive()
{

}

QSqlQuery* Archive::connect(){
    QFile file("/home/zdw/QT/DWChat/Algorithm/app.config");
    file.open(QIODevice::ReadOnly);
    QJsonDocument jsonDocument = QJsonDocument::fromJson(file.readAll());
    QVariant variant = jsonDocument.toVariant();
    QMap<QString, QVariant> map = variant.toMap();

    QSqlDatabase db = QSqlDatabase::addDatabase(map["sql"].toMap()["Type"].toString().toUtf8());
    db.setHostName(map["sql"].toMap()["HostName"].toString().toUtf8());
    db.setDatabaseName(map["sql"].toMap()["DatabaseName"].toString().toUtf8());
    db.setUserName(map["sql"].toMap()["UserName"].toString().toUtf8());
    db.setPassword(map["sql"].toMap()["Password"].toString().toUtf8());

    if (db.open()) qDebug() << "Connected to MySQL database!";
    else{
        qDebug() << "Connected to MySQL database Error!";
        exit(1);
    }

    // 创建查询对象
    QSqlQuery* query = new QSqlQuery(db);
    return query;
}
