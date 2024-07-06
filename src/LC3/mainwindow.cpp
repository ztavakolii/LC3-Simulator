#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QIcon>
#include <QTableWidgetItem>
#include <QString>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    QIcon windowsIcon(":/fonts & pictures/images/icon.png");
    this->setWindowIcon(windowsIcon);

    setWindowTitle("LC3 Simulator");


    ui->memoryTable->horizontalHeader()->setStyleSheet("QHeaderView::section{ background-color:rgb(28,97,135);}");
    ui->memoryTable->setHorizontalHeaderLabels({"Dec Address","Hex Address","Contents"});
    for(int i=0;i<65536;i++){
        QTableWidgetItem* item1=new QTableWidgetItem();
        item1->setText(QString::number(i));
        item1->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
        ui->memoryTable->setItem(i,0,item1);

        QTableWidgetItem* item2=new QTableWidgetItem();
        item2->setText(QString::number(i,16).toUpper());
        item2->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
        ui->memoryTable->setItem(i,1,item2);

        QTableWidgetItem* item3=new QTableWidgetItem();
        item3->setText("NOP");
        item3->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
        ui->memoryTable->setItem(i,2,item3);

    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

