#ifndef ARCHIVE_H
#define ARCHIVE_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlDriver>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>

class Archive
{
public:
    Archive();
    QSqlQuery* connect();
};

#endif // ARCHIVE_H
