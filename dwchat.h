#ifndef DWCHAT_H
#define DWCHAT_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class DWChat; }
QT_END_NAMESPACE

class DWChat : public QWidget
{
    Q_OBJECT

public:
    DWChat(QWidget *parent = nullptr);
    ~DWChat();

private:
    Ui::DWChat *ui;
};
#endif // DWCHAT_H
