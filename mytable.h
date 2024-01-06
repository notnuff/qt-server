#ifndef MYTABLE_H
#define MYTABLE_H

#include <QWidget>
#include <QTableWidget>

namespace Ui {
class MyTable;
}

class MyTable : public QWidget
{
    Q_OBJECT

public:
    explicit MyTable(QWidget *parent = nullptr);
    ~MyTable();

private:
    Ui::MyTable *ui;
    QTableView *tView;
    QTableWidget *tWidget;
};

#endif // MYTABLE_H
