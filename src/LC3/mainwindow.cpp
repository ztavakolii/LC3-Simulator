#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QIcon>
#include <QTableWidgetItem>
#include <QString>
#include <QTextCharFormat>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <unordered_map>
using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(1280,700);

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


void MainWindow::on_codeEditor_currentCharFormatChanged(const QTextCharFormat &format)
{
    QTextCursor cursor=ui->codeEditor->textCursor();
    QTextCharFormat _format_;
    _format_.setFont(QFont("Ready to Party"));
    cursor.setCharFormat(_format_);
    ui->codeEditor->setTextCursor(cursor);
}


void stringToInt(QString&s,int&res){
    if(s[0]==',')
        s.remove(0,1);

    if(s[0]=='#'){
        s.remove(0,1);
        res=s.toInt(nullptr,10);
    }
    else if(s[0]=='b'){
        s.remove(0,1);
        res=s.toInt(nullptr,2);
    }
    else if(s[0]=='x'){
        s.remove(0,1);
        res=s.toInt(nullptr,16);
    }
}

void MainWindow::on_assembleButton_clicked()
{
    unordered_map<QString,QString> symbolTable;
    symbolTable.insert({"ADD","0001"});
    symbolTable.insert({"AND","0101"});
    symbolTable.insert({"NOT","1001"});
    symbolTable.insert({"LD","0010"});
    symbolTable.insert({"LDI","1010"});
    symbolTable.insert({"LDR","0110"});
    symbolTable.insert({"LEA","1110"});
    symbolTable.insert({"ST","0011"});
    symbolTable.insert({"STI","1011"});
    symbolTable.insert({"STR","0111"});
    symbolTable.insert({"RET","1100000111000000"});
    symbolTable.insert({"JMP","1100000"});
    symbolTable.insert({"JSR","01001"});
    symbolTable.insert({"JSRR","0100000"});
    symbolTable.insert({"BRn","0000100"});
    symbolTable.insert({"BRz","0000010"});
    symbolTable.insert({"BRp","0000001"});
    symbolTable.insert({"BRzp","0000011"});
    symbolTable.insert({"BRnp","0000101"});
    symbolTable.insert({"BRnz","0000110"});
    symbolTable.insert({"BRnzp","0000111"});
    symbolTable.insert({"BR","0000000"});
    symbolTable.insert({"R0","000"});
    symbolTable.insert({"R1","001"});
    symbolTable.insert({"R2","010"});
    symbolTable.insert({"R3","011"});
    symbolTable.insert({"R4","100"});
    symbolTable.insert({"R5","101"});
    symbolTable.insert({"R6","110"});
    symbolTable.insert({"R7","111"});
    symbolTable.insert({"R0,","000"});
    symbolTable.insert({"R1,","001"});
    symbolTable.insert({"R2,","010"});
    symbolTable.insert({"R3,","011"});
    symbolTable.insert({"R4,","100"});
    symbolTable.insert({"R5,","101"});
    symbolTable.insert({"R6,","110"});
    symbolTable.insert({"R7,","111"});


    QString text=ui->codeEditor->toPlainText();
    QStringList lines=text.split('\n');

    //First review
    int memoryCellNumber=0,temp;
    int l=lines.size();
    for(int i=0;i<l;i++){
        QStringList sections=lines[i].split(' ');
        if(sections[0]=="ORG"){
            stringToInt(sections[1],memoryCellNumber);
            temp=memoryCellNumber;
            memoryCellNumber--;
        }
        else{
            if(sections[0][0]!=';'){
                memoryCellNumber++;
                if(symbolTable.find(sections[0])==symbolTable.end()){
                    sections[0].remove(',');  //posibility of error)
                    symbolTable.insert({sections[0],QString::number(memoryCellNumber,2)});
                    symbolTable.insert({sections[0]+",",QString::number(memoryCellNumber,2)});
                }

            }
        }
    }

    //second review
    memoryCellNumber=temp;



}

