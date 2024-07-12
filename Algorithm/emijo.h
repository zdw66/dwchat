#ifndef EMIJO_H
#define EMIJO_H

#include <QWidget>
#include <QObject>
#include <QApplication>
#include <QVBoxLayout>
#include <QComboBox>
#include <QTableWidget>
#include <QHeaderView>
#include <QObject>
#include <QDebug>

namespace Ui {
class Emijo;
}

class Emijo : public QWidget
{
    Q_OBJECT

public:
    explicit Emijo(QWidget *parent = nullptr);
    ~Emijo();
signals:
    QString getEmijo(QString);

private:
    Ui::Emijo *ui;
};

#endif // EMIJO_H
