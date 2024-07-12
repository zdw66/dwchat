#include "emijo.h"
#include "ui_emijo.h"

Emijo::Emijo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Emijo)
{
    ui->setupUi(this);
    this->resize(245,245);
    // 创建表情包下拉表格框
    ui->tableWidget->setColumnCount(6);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    // 隐藏水平表头和垂直表头
    ui->tableWidget->horizontalHeader()->hide();
    ui->tableWidget->verticalHeader()->hide();
//    tableWidget.horizontalHeader()->setStyleSheet("QHeaderView::section { border: none; }");
    ui->tableWidget->setColumnWidth(0, 40); // 图标列的宽度
    ui->tableWidget->setColumnWidth(1, 40); // 图标列的宽度
    ui->tableWidget->setColumnWidth(2, 40); // 图标列的宽度
    ui->tableWidget->setColumnWidth(3, 40); // 图标列的宽度
    ui->tableWidget->setColumnWidth(4, 40); // 图标列的宽度
    ui->tableWidget->setColumnWidth(5, 40); // 图标列的宽度
    int k=12;
    QStringList emojis = {"😀","😁","😂","🤣","😃","😄","😅","😆","😉","😊","😋","😎",
                          "😙","🥲","🤗","🤩","🤔","🫡","🤨","😐","😑","😶","🫥","🙄",
                          "😮","🤐","😯","😪","😫","🥱","😴","😌","😛","😜","😝","🤤",
                          "🫤","🙃","🫠","🤑","😖","😞","😟","😤","😢","😭","😦","😧",
                          "💨","😰","😱","🥵","🥶","😳","🤪","😵","😵‍","🥴","😠","😡",
                          "🤢","🤮","🤧","😇","🥳","🥸","🥺","🥹","🤠","🤡","🤥","🤫",
                          "😍","😘","🥰","😗","😏","😣","😥","🤓","😒","😓","😔","😕",
                          "😨","😩","🤯","😬","🤬","😷","🤒","🤕","🤭","🫢","🫣","🧐"}; // 表情包图片文件路径
    for (const QString &emoji : emojis) {
        QTableWidgetItem *iconItem = new QTableWidgetItem(emoji);
        iconItem->setTextAlignment(Qt::AlignCenter);
        QFont font = iconItem->font();
        font.setPointSize(20); // 设置字体大小为20

        // 将新的字体设置为 QTableWidgetItem 的字体
        iconItem->setFont(font);
        if(k==12){
            ui->tableWidget->insertRow(ui->tableWidget->rowCount());
            k%=12;
        }
        ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, k, iconItem);
        k++;
    }
    std::function<void(int,int)>  handleCellClick = [&](int row, int column) {
        QTableWidgetItem *item = ui->tableWidget->item(row, column);
        if(item) {
            qDebug() << "Cell clicked at row: " << row << " column: " << column << " with text: " << item->text();
            emit getEmijo(item->text());
        }
    };
    // 连接表格中的单元格点击事件
    QObject::connect(ui->tableWidget, &QTableWidget::cellClicked, ui->tableWidget, handleCellClick);

}

Emijo::~Emijo()
{
    delete ui;
}
