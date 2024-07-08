#ifndef FILEVIEW_H
#define FILEVIEW_H

#include <QWidget>

namespace Ui {
class FileView;
}

class FileView : public QWidget
{
    Q_OBJECT

public:
    explicit FileView(QWidget *parent = nullptr);
    ~FileView();

private:
    Ui::FileView *ui;

public:
    void setView(QString fileName,QString fileSize);
};

#endif // FILEVIEW_H
