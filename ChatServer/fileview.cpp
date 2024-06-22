#include "fileview.h"
#include "ui_fileview.h"

FileView::FileView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileView)
{
    ui->setupUi(this);
}

FileView::~FileView()
{
    delete ui;
}

void FileView::setView(QString fileName, QString fileSize){
    int a =0;
    if(fileName.endsWith(".zip")
     ||fileName.endsWith(".rar")
     ||fileName.endsWith(".tar.gz")){
        a=1;
    }else if (fileName.endsWith(".txt")){
        a=2;
    }else a=3;

    switch (a) {
    case 1:
        ui->label->setStyleSheet("border-image: url(:/img/yasuo.png);");

        break;
    case 2:
        ui->label->setStyleSheet("border-image: url(:/img/txt_planer.png);");
        break;
    case 3:
        ui->label->setStyleSheet("border-image: url(:/img/weizhi.png);");
        break;
    }
    ui->label_2->setText(fileName);
    ui->label_3->setText(fileSize);
}
