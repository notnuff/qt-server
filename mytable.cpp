#include "mytable.h"
#include "ui_mytable.h"

MyTable::MyTable(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyTable)
{
    tView = ui->tableView;
    tWidget = new QTableWidget;
    ui->setupUi(this);
}

MyTable::~MyTable()
{
    delete ui;
}
