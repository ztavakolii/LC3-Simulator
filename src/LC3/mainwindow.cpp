#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QIcon>
#include <QTableWidgetItem>
#include <QString>
#include <QTextCharFormat>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <unordered_map>
#include <bitset>
#include <QMessageBox>
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
    ui->helpTextEdit->setFontPointSize(13);
    ui->helpTextEdit->setText("\nGuide:\n\nThis program is a simulator for LC3 computer.\n\n"
"This computer has 8 general-purpose registers in the central processing unit and 4 special-purpose registers such as PC, IR, MAR, MDR and a common code and data memory (Von Neumann) with dimensions of 64K*16.\n\n"
"This computer is used for general processing applications.\n"
"When your program is executed, you can see the content of each register and memory house after executing each command.\n\n"
"How to execute our program?\n\n"
"1.place your code in editor.\n"
"2.click on Assemble button.\n"
"3.for every instruction, click on fetch button then decode button then execute button.\n\n"
"NOTE: you should keep the order of clicking on buttons otherwise it wont work properly.\n\n"
"You can see the block diagram of this computer in the picture on the right.\n");
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

QString signExtend(QString binString,int newLength){
    int diff=newLength-binString.length();
    for(int i=0;i<diff;i++){
        binString=binString[0]+binString;
    }
    return binString;
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

QString getTwosComplement(int num, int bits) {
    num=num*-1;
    std::vector<bool> binary(bits);
    for (int i = 0; i < bits; ++i) {
        binary[i] = (num >> i) & 1;
    }
    for (int i = 0; i < bits; ++i) {
        binary[i] = !binary[i];
    }
    for (int i = 0; i < bits; ++i) {
        if (binary[i] == 0) {
            binary[i] = 1;
            break;
        }
        else {
            binary[i] = 0;
        }
    }
    QString s="";
    for(int i=binary.size()-1;i>=0;i--){
        s+=binary[i]? '1':'0';
    }
    return s;
}


void MainWindow::on_assembleButton_clicked()
{
    unordered_map<QString,pair<int,QString>> symbolTable;
    symbolTable.insert({"ADD",{1,"0001"}});
    symbolTable.insert({"AND",{1,"0101"}});
    symbolTable.insert({"NOT",{1,"1001"}});
    symbolTable.insert({"LD",{1,"0010"}});
    symbolTable.insert({"LDI",{1,"1010"}});
    symbolTable.insert({"LDR",{1,"0110"}});
    symbolTable.insert({"LEA",{1,"1110"}});
    symbolTable.insert({"ST",{1,"0011"}});
    symbolTable.insert({"STI",{1,"1011"}});
    symbolTable.insert({"STR",{1,"0111"}});
    symbolTable.insert({"RET",{1,"1100000111000000"}});
    symbolTable.insert({"JMP",{1,"1100000"}});
    symbolTable.insert({"JSR",{1,"01001"}});
    symbolTable.insert({"JSRR",{1,"0100000"}});
    symbolTable.insert({"BRn",{1,"0000100"}});
    symbolTable.insert({"BRz",{1,"0000010"}});
    symbolTable.insert({"BRp",{1,"0000001"}});
    symbolTable.insert({"BRzp",{1,"0000011"}});
    symbolTable.insert({"BRnp",{1,"0000101"}});
    symbolTable.insert({"BRnz",{1,"0000110"}});
    symbolTable.insert({"BRnzp",{1,"0000111"}});
    symbolTable.insert({"BR",{1,"0000000"}});
    symbolTable.insert({"R0",{1,"000"}});
    symbolTable.insert({"R1",{1,"001"}});
    symbolTable.insert({"R2",{1,"010"}});
    symbolTable.insert({"R3",{1,"011"}});
    symbolTable.insert({"R4",{1,"100"}});
    symbolTable.insert({"R5",{1,"101"}});
    symbolTable.insert({"R6",{1,"110"}});
    symbolTable.insert({"R7",{1,"111"}});
    symbolTable.insert({"R0,",{1,"000"}});
    symbolTable.insert({"R1,",{1,"001"}});
    symbolTable.insert({"R2,",{1,"010"}});
    symbolTable.insert({"R3,",{1,"011"}});
    symbolTable.insert({"R4,",{1,"100"}});
    symbolTable.insert({"R5,",{1,"101"}});
    symbolTable.insert({"R6,",{1,"110"}});
    symbolTable.insert({"R7,",{1,"111"}});
    symbolTable.insert({"HALT",{1,"1111000000100101"}});

    QString text=ui->codeEditor->toPlainText();
    QStringList lines=text.split('\n');

    //First review
    int memoryCellNumber=0,temp;
    int l=lines.size();
    for(int i=0;i<l;i++){
        QStringList sections=lines[i].split(' ');
        if(sections[0]!="END"){
        if(sections[0]=="ORG"){
            stringToInt(sections[1],memoryCellNumber);
            temp=memoryCellNumber;
            memoryCellNumber--;
        }
        else{
            if(sections[0][0]!=';'){
                memoryCellNumber++;
                if(symbolTable.find(sections[0])==symbolTable.end()
                    && sections[1]!="DEC" && sections[1]!="HEX"){
                    sections[0].remove(',');  //posibility of error)
                    symbolTable.insert({sections[0],{0,(QString::number(memoryCellNumber))}});
                    symbolTable.insert({sections[0]+",",{0,(QString::number(memoryCellNumber))}});
                }
                else if(symbolTable.find(sections[0])==symbolTable.end()
                           && (sections[1]=="DEC" || sections[1]=="HEX")){
                    sections[0].remove(',');
                    if(sections[1]=="DEC"){
                        int res=sections[2].toInt(nullptr,10);
                        symbolTable.insert({sections[0],{0,QString::number(res)}});
                        symbolTable.insert({sections[0]+",",{0,QString::number(res)}});
                    }
                    else if(sections[0]=="HEX"){
                        int res=sections[2].toInt(nullptr,16);
                        symbolTable.insert({sections[0],{0,QString::number(res)}});
                        symbolTable.insert({sections[0]+",",{0,QString::number(res)}});
                    }
                }
            }
        }
        }
    }

    //second review

    memoryCellNumber=temp;
    memoryCellNumber--;
    for(int i=0;i<l;i++){
        QStringList sections=lines[i].split(' ');
        if(symbolTable.find(sections[0])!=symbolTable.end()){
            QString binString="";
            if(symbolTable[sections[0]].first==1){
                if(sections[0]=="ADD"){
                    if(sections.size()!=4){
                        QMessageBox message;
                        message.setText("The format of the ADD command is incorrect.");
                        message.setWindowIcon(QIcon(":/fonts & pictures/images/icon.png"));
                        message.setStyleSheet("color: rgb(255, 255, 255); background-color: rgb(25,35,75);");
                        message.setIcon(QMessageBox::Critical);
                        message.exec();
                        i--;
                        break;
                    }
                    else{
                    binString+=symbolTable["ADD"].second;
                    binString+=symbolTable[sections[1]].second;
                    binString+=symbolTable[sections[2]].second;
                    if(symbolTable.find(sections[3])!=symbolTable.end()){
                        binString+="000";
                        binString+=symbolTable[sections[3]].second;
                    }
                    else{
                        binString+="1";
                        int res=0;
                        stringToInt(sections[3],res);
                        if(res<-16||res>15){
                            QMessageBox message;
                            message.setText("The number entered for the ADD command must be in the range of -16 to 15.");
                            message.setWindowIcon(QIcon(":/fonts & pictures/images/icon.png"));
                            message.setStyleSheet("color: rgb(255, 255, 255); background-color: rgb(25,35,75);");
                            message.setIcon(QMessageBox::Critical);
                            message.exec();
                            i--;
                            break;
                        }
                        else{
                            binString+=getTwosComplement(res,5);
                        }
                    }
                    QTableWidgetItem* item=new QTableWidgetItem();
                    int res=binString.toInt(nullptr,2);
                    item->setText(QString::number(res,16).toUpper());
                    item->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
                    ui->memoryTable->setItem(++memoryCellNumber,2,item);
                }
                }

                else if(sections[0]=="AND"){
                    if(sections.size()!=4){
                        QMessageBox message;
                        message.setText("The format of the AND command is incorrect.");
                        message.setWindowIcon(QIcon(":/fonts & pictures/images/icon.png"));
                        message.setStyleSheet("color: rgb(255, 255, 255); background-color: rgb(25,35,75);");
                        message.setIcon(QMessageBox::Critical);
                        message.exec();
                        i--;
                        break;
                    }
                    else{
                        binString+=symbolTable["AND"].second;
                        binString+=symbolTable[sections[1]].second;
                        binString+=symbolTable[sections[2]].second;
                        if(symbolTable.find(sections[3])!=symbolTable.end()){
                            binString+="000";
                            binString+=symbolTable[sections[3]].second;
                        }
                        else{
                            binString+="1";
                            int res=0;
                            stringToInt(sections[3],res);
                            if(res<-16||res>15){
                                QMessageBox message;
                                message.setText("The number entered for the AND command must be in the range of -16 to 15.");
                                message.setWindowIcon(QIcon(":/fonts & pictures/images/icon.png"));
                                message.setStyleSheet("color: rgb(255, 255, 255); background-color: rgb(25,35,75);");
                                message.setIcon(QMessageBox::Critical);
                                message.exec();
                                i--;
                                break;
                            }
                            else{
                                binString+=getTwosComplement(res,5);
                            }
                        }
                        QTableWidgetItem* item=new QTableWidgetItem();
                        int res=binString.toInt(nullptr,2);
                        item->setText(QString::number(res,16).toUpper());
                        item->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
                        ui->memoryTable->setItem(++memoryCellNumber,2,item);
                    }
                }

                else if(sections[0]=="NOT"){
                    if(sections.size()!=3){
                        QMessageBox message;
                        message.setText("The format of the NOT command is incorrect.");
                        message.setWindowIcon(QIcon(":/fonts & pictures/images/icon.png"));
                        message.setStyleSheet("color: rgb(255, 255, 255); background-color: rgb(25,35,75);");
                        message.setIcon(QMessageBox::Critical);
                        message.exec();
                        i--;
                        break;
                    }
                    else{
                    binString+=symbolTable["NOT"].second;
                    binString+=symbolTable[sections[1]].second;
                    binString+=symbolTable[sections[2]].second;
                    binString+="111111";
                    QTableWidgetItem* item=new QTableWidgetItem();
                    int res=binString.toInt(nullptr,2);
                    item->setText(QString::number(res,16).toUpper());
                    item->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
                    ui->memoryTable->setItem(++memoryCellNumber,2,item);
                    }
                }
                else if(sections[0]=="LD"){
                    if(sections.size()!=3){
                    QMessageBox message;
                    message.setText("The format of the LD command is incorrect.");
                    message.setWindowIcon(QIcon(":/fonts & pictures/images/icon.png"));
                    message.setStyleSheet("color: rgb(255, 255, 255); background-color: rgb(25,35,75);");
                    message.setIcon(QMessageBox::Critical);
                    message.exec();
                    i--;
                    break;
                    }
                    else{
                    binString+=symbolTable["LD"].second;
                    binString+=symbolTable[sections[1]].second;
                    if(symbolTable.find(sections[2])!=symbolTable.end()){
                       // binString+=signExtend(symbolTable[sections[2]].second,9);
                        binString+=getTwosComplement(symbolTable[sections[2]].second.toInt(),9);
                    }
                    else{
                        int res=0;
                        stringToInt(sections[2],res);
                        binString+=getTwosComplement(res,9);
                    }
                    QTableWidgetItem* item=new QTableWidgetItem();
                    int res=binString.toInt(nullptr,2);
                    item->setText(QString::number(res,16).toUpper());
                    item->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
                    ui->memoryTable->setItem(++memoryCellNumber,2,item);
                    }
                }
                else if(sections[0]=="LDI"){
                    if(sections.size()!=3){
                    QMessageBox message;
                    message.setText("The format of the LDI command is incorrect.");
                    message.setWindowIcon(QIcon(":/fonts & pictures/images/icon.png"));
                    message.setStyleSheet("color: rgb(255, 255, 255); background-color: rgb(25,35,75);");
                    message.setIcon(QMessageBox::Critical);
                    message.exec();
                    i--;
                    break;
                    }
                    else{
                    binString+=symbolTable["LDI"].second;
                    binString+=symbolTable[sections[1]].second;
                    if(symbolTable.find(sections[2])!=symbolTable.end()){
                        binString+=getTwosComplement(symbolTable[sections[2]].second.toInt(),9);
                    }
                    else{
                        int res=0;
                        stringToInt(sections[2],res);
                        binString+=getTwosComplement(res,9);
                    }
                    QTableWidgetItem* item=new QTableWidgetItem();
                    int res=binString.toInt(nullptr,2);
                    item->setText(QString::number(res,16).toUpper());
                    item->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
                    ui->memoryTable->setItem(++memoryCellNumber,2,item);
                    }
                }
                else if(sections[0]=="LDR"){
                    if(sections.size()!=4){
                    QMessageBox message;
                    message.setText("The format of the LDR command is incorrect.");
                    message.setWindowIcon(QIcon(":/fonts & pictures/images/icon.png"));
                    message.setStyleSheet("color: rgb(255, 255, 255); background-color: rgb(25,35,75);");
                    message.setIcon(QMessageBox::Critical);
                    message.exec();
                    i--;
                    break;
                    }
                    else{
                    binString+=symbolTable["LDR"].second;
                    binString+=symbolTable[sections[1]].second;
                    binString+=symbolTable[sections[2]].second;
                    if(symbolTable.find(sections[3])!=symbolTable.end()){
                        binString+=getTwosComplement(symbolTable[sections[3]].second.toInt(),6);
                    }
                    else{
                        int res=0;
                        stringToInt(sections[3],res);
                        binString+=getTwosComplement(res,6);
                    }
                    QTableWidgetItem* item=new QTableWidgetItem();
                    int res=binString.toInt(nullptr,2);
                    item->setText(QString::number(res,16).toUpper());
                    item->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
                    ui->memoryTable->setItem(++memoryCellNumber,2,item);
                    }
                    }

                else if(sections[0]=="LEA"){
                    if(sections.size()!=3){
                    QMessageBox message;
                    message.setText("The format of the LEA command is incorrect.");
                    message.setWindowIcon(QIcon(":/fonts & pictures/images/icon.png"));
                    message.setStyleSheet("color: rgb(255, 255, 255); background-color: rgb(25,35,75);");
                    message.setIcon(QMessageBox::Critical);
                    message.exec();
                    i--;
                    break;
                    }
                    else{
                    binString+=symbolTable["LEA"].second;
                    binString+=symbolTable[sections[1]].second;
                    if(symbolTable.find(sections[2])!=symbolTable.end()){
                    binString+=getTwosComplement(symbolTable[sections[2]].second.toInt(),9);
                    }
                    else{
                    int res=0;
                    stringToInt(sections[2],res);
                    binString+=getTwosComplement(res,9);
                    }
                    QTableWidgetItem* item=new QTableWidgetItem();
                    int res=binString.toInt(nullptr,2);
                    item->setText(QString::number(res,16).toUpper());
                    item->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
                    ui->memoryTable->setItem(++memoryCellNumber,2,item);
                    }
                }
                else if(sections[0]=="ST"){
                    if(sections.size()!=3){
                    QMessageBox message;
                    message.setText("The format of the ST command is incorrect.");
                    message.setWindowIcon(QIcon(":/fonts & pictures/images/icon.png"));
                    message.setStyleSheet("color: rgb(255, 255, 255); background-color: rgb(25,35,75);");
                    message.setIcon(QMessageBox::Critical);
                    message.exec();
                    i--;
                    break;
                    }
                    else{
                    binString+=symbolTable["ST"].second;
                    binString+=symbolTable[sections[1]].second;
                    if(symbolTable.find(sections[2])!=symbolTable.end()){
                    binString+=getTwosComplement(symbolTable[sections[2]].second.toInt(),9);
                    }
                    else{
                    int res=0;
                    stringToInt(sections[2],res);
                    binString+=getTwosComplement(res,9);
                    }
                    QTableWidgetItem* item=new QTableWidgetItem();
                    int res=binString.toInt(nullptr,2);
                    item->setText(QString::number(res,16).toUpper());
                    item->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
                    ui->memoryTable->setItem(++memoryCellNumber,2,item);
                    }
                }
                else if(sections[0]=="STI"){
                    if(sections.size()!=3){
                    QMessageBox message;
                    message.setText("The format of the STI command is incorrect.");
                    message.setWindowIcon(QIcon(":/fonts & pictures/images/icon.png"));
                    message.setStyleSheet("color: rgb(255, 255, 255); background-color: rgb(25,35,75);");
                    message.setIcon(QMessageBox::Critical);
                    message.exec();
                    i--;
                    break;
                    }
                    else{
                    binString+=symbolTable["STI"].second;
                    binString+=symbolTable[sections[1]].second;
                    if(symbolTable.find(sections[2])!=symbolTable.end()){
                    binString+=getTwosComplement(symbolTable[sections[2]].second.toInt(),9);
                    }
                    else{
                    int res=0;
                    stringToInt(sections[2],res);
                    binString+=getTwosComplement(res,9);
                    }
                    QTableWidgetItem* item=new QTableWidgetItem();
                    int res=binString.toInt(nullptr,2);
                    item->setText(QString::number(res,16).toUpper());
                    item->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
                    ui->memoryTable->setItem(++memoryCellNumber,2,item);
                    }
                }
                else if(sections[0]=="STR"){
                    if(sections.size()!=4){
                    QMessageBox message;
                    message.setText("The format of the STR command is incorrect.");
                    message.setWindowIcon(QIcon(":/fonts & pictures/images/icon.png"));
                    message.setStyleSheet("color: rgb(255, 255, 255); background-color: rgb(25,35,75);");
                    message.setIcon(QMessageBox::Critical);
                    message.exec();
                    i--;
                    break;
                    }
                    else{
                    binString+=symbolTable["STR"].second;
                    binString+=symbolTable[sections[1]].second;
                    binString+=symbolTable[sections[2]].second;
                    if(symbolTable.find(sections[3])!=symbolTable.end()){
                    binString+=getTwosComplement(symbolTable[sections[3]].second.toInt(),6);
                    }
                    else{
                    int res=0;
                    stringToInt(sections[3],res);
                    binString+=getTwosComplement(res,6);
                    }
                    QTableWidgetItem* item=new QTableWidgetItem();
                    int res=binString.toInt(nullptr,2);
                    item->setText(QString::number(res,16).toUpper());
                    item->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
                    ui->memoryTable->setItem(++memoryCellNumber,2,item);
                    }
                }

                else if(sections[0].contains("BR")){
                if(sections.size()!=2){
                    QMessageBox message;
                    message.setText("The format of the BR command is incorrect.");
                    message.setWindowIcon(QIcon(":/fonts & pictures/images/icon.png"));
                    message.setStyleSheet("color: rgb(255, 255, 255); background-color: rgb(25,35,75);");
                    message.setIcon(QMessageBox::Critical);
                    message.exec();
                    i--;
                    break;
                }
                else{
                    binString+=symbolTable[sections[0]].second;
                    if(symbolTable.find(sections[1])!=symbolTable.end()){
                    binString+=getTwosComplement(symbolTable[sections[1]].second.toInt(),9);
                    }
                    else{
                    int res=0;
                    stringToInt(sections[1],res);
                    binString+=getTwosComplement(res,9);
                    }
                    QTableWidgetItem* item=new QTableWidgetItem();
                    int res=binString.toInt(nullptr,2);
                    item->setText(QString::number(res,16).toUpper());
                    item->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
                    ui->memoryTable->setItem(++memoryCellNumber,2,item);
                }
                }
                else if(sections[0]=="JMP"){
                if(sections.size()!=2){
                    QMessageBox message;
                    message.setText("The format of the JMP command is incorrect.");
                    message.setWindowIcon(QIcon(":/fonts & pictures/images/icon.png"));
                    message.setStyleSheet("color: rgb(255, 255, 255); background-color: rgb(25,35,75);");
                    message.setIcon(QMessageBox::Critical);
                    message.exec();
                    i--;
                    break;
                }
                else{
                    binString+=symbolTable["JMP"].second;
                    binString+=symbolTable[sections[1]].second;
                    binString+="000000";
                    QTableWidgetItem* item=new QTableWidgetItem();
                    int res=binString.toInt(nullptr,2);
                    item->setText(QString::number(res,16).toUpper());
                    item->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
                    ui->memoryTable->setItem(++memoryCellNumber,2,item);
                }
                }
                else if(sections[0]=="RET"){
                if(sections.size()!=1){
                    QMessageBox message;
                    message.setText("The format of the RET command is incorrect.");
                    message.setWindowIcon(QIcon(":/fonts & pictures/images/icon.png"));
                    message.setStyleSheet("color: rgb(255, 255, 255); background-color: rgb(25,35,75);");
                    message.setIcon(QMessageBox::Critical);
                    message.exec();
                    i--;
                    break;
                }
                else{
                    binString+=symbolTable["RET"].second;
                    QTableWidgetItem* item=new QTableWidgetItem();
                    int res=binString.toInt(nullptr,2);
                    item->setText(QString::number(res,16).toUpper());
                    item->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
                    ui->memoryTable->setItem(++memoryCellNumber,2,item);
                }
                }
                else if(sections[0]=="JSR"){
                if(sections.size()!=2){
                    QMessageBox message;
                    message.setText("The format of the JSR command is incorrect.");
                    message.setWindowIcon(QIcon(":/fonts & pictures/images/icon.png"));
                    message.setStyleSheet("color: rgb(255, 255, 255); background-color: rgb(25,35,75);");
                    message.setIcon(QMessageBox::Critical);
                    message.exec();
                    i--;
                    break;
                }
                else{
                    binString+=symbolTable["JSR"].second;
                    if(symbolTable.find(sections[1])!=symbolTable.end()){
                    binString+=getTwosComplement(symbolTable[sections[1]].second.toInt(),11);
                    }
                    else{
                    int res=0;
                    stringToInt(sections[1],res);
                    binString+=getTwosComplement(res,11);
                    }
                    QTableWidgetItem* item=new QTableWidgetItem();
                    int res=binString.toInt(nullptr,2);
                    item->setText(QString::number(res,16).toUpper());
                    item->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
                    ui->memoryTable->setItem(++memoryCellNumber,2,item);
                }
                }
                else if(sections[0]=="JSRR"){
                if(sections.size()!=2){
                    QMessageBox message;
                    message.setText("The format of the JSRR command is incorrect.");
                    message.setWindowIcon(QIcon(":/fonts & pictures/images/icon.png"));
                    message.setStyleSheet("color: rgb(255, 255, 255); background-color: rgb(25,35,75);");
                    message.setIcon(QMessageBox::Critical);
                    message.exec();
                    i--;
                    break;
                }
                else{
                    binString+=symbolTable["JSRR"].second;
                    binString+=symbolTable[sections[1]].second;
                    binString+="000000";
                    QTableWidgetItem* item=new QTableWidgetItem();
                    int res=binString.toInt(nullptr,2);
                    item->setText(QString::number(res,16).toUpper());
                    item->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
                    ui->memoryTable->setItem(++memoryCellNumber,2,item);
                }
                }
                else if(sections[0]=="HALT"){
                if(sections.size()!=1){
                    QMessageBox message;
                    message.setText("The format of the HALT command is incorrect.");
                    message.setWindowIcon(QIcon(":/fonts & pictures/images/icon.png"));
                    message.setStyleSheet("color: rgb(255, 255, 255); background-color: rgb(25,35,75);");
                    message.setIcon(QMessageBox::Critical);
                    message.exec();
                    i--;
                    break;
                }
                else{
                    binString+=symbolTable["HALT"].second;
                    QTableWidgetItem* item=new QTableWidgetItem();
                    int res=binString.toInt(nullptr,2);
                    item->setText(QString::number(res,16).toUpper());
                    item->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
                    ui->memoryTable->setItem(++memoryCellNumber,2,item);
                }
                }
            }
            else if(symbolTable[sections[0]].first==0){
                if(sections[1]=="ADD"){
                    if(sections.size()!=5){
                    QMessageBox message;
                    message.setText("The format of the ADD command is incorrect.");
                    message.setWindowIcon(QIcon(":/fonts & pictures/images/icon.png"));
                    message.setStyleSheet("color: rgb(255, 255, 255); background-color: rgb(25,35,75);");
                    message.setIcon(QMessageBox::Critical);
                    message.exec();
                    i--;
                    break;
                    }
                    else{
                    binString+=symbolTable["ADD"].second;
                    binString+=symbolTable[sections[2]].second;
                    binString+=symbolTable[sections[3]].second;
                    if(symbolTable.find(sections[4])!=symbolTable.end()){
                        binString+="000";
                        binString+=symbolTable[sections[4]].second;
                    }
                    else{
                        binString+="1";
                        int res=0;
                        stringToInt(sections[4],res);
                        if(res<-16||res>15){
                            QMessageBox message;
                            message.setText("The number entered for the ADD command must be in the range of -16 to 15.");
                            message.setWindowIcon(QIcon(":/fonts & pictures/images/icon.png"));
                            message.setStyleSheet("color: rgb(255, 255, 255); background-color: rgb(25,35,75);");
                            message.setIcon(QMessageBox::Critical);
                            message.exec();
                            i--;
                            break;
                        }
                        else{
                            binString+=getTwosComplement(res,5);

                        }
                    }
                    QTableWidgetItem* item=new QTableWidgetItem();
                    int res=binString.toInt(nullptr,2);
                    item->setText(QString::number(res,16).toUpper());
                    item->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
                    ui->memoryTable->setItem(++memoryCellNumber,2,item);
                    }

                }

                else if(sections[1]=="AND"){
                    if(sections.size()!=5){
                    QMessageBox message;
                    message.setText("The format of the AND command is incorrect.");
                    message.setWindowIcon(QIcon(":/fonts & pictures/images/icon.png"));
                    message.setStyleSheet("color: rgb(255, 255, 255); background-color: rgb(25,35,75);");
                    message.setIcon(QMessageBox::Critical);
                    message.exec();
                    i--;
                    break;
                    }
                    else{
                    binString+=symbolTable["AND"].second;
                    binString+=symbolTable[sections[2]].second;
                    binString+=symbolTable[sections[3]].second;
                    if(symbolTable.find(sections[4])!=symbolTable.end()){
                        binString+="000";
                        binString+=symbolTable[sections[4]].second;
                    }
                    else{
                        binString+="1";
                        int res=0;
                        stringToInt(sections[4],res);
                        if(res<-16||res>15){
                            QMessageBox message;
                            message.setText("The number entered for the AND command must be in the range of -16 to 15.");
                            message.setWindowIcon(QIcon(":/fonts & pictures/images/icon.png"));
                            message.setStyleSheet("color: rgb(255, 255, 255); background-color: rgb(25,35,75);");
                            message.setIcon(QMessageBox::Critical);
                            message.exec();
                            i--;
                            break;
                        }
                        else{
                            binString+=getTwosComplement(res,5);
                        }
                    }
                    QTableWidgetItem* item=new QTableWidgetItem();
                    int res=binString.toInt(nullptr,2);
                    item->setText(QString::number(res,16).toUpper());
                    item->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
                    ui->memoryTable->setItem(++memoryCellNumber,2,item);
                    }

                }
                else if(sections[1]=="NOT"){
                    if(sections.size()!=4){
                    QMessageBox message;
                    message.setText("The format of the NOT command is incorrect.");
                    message.setWindowIcon(QIcon(":/fonts & pictures/images/icon.png"));
                    message.setStyleSheet("color: rgb(255, 255, 255); background-color: rgb(25,35,75);");
                    message.setIcon(QMessageBox::Critical);
                    message.exec();
                    i--;
                    break;
                    }
                    else{
                    binString+=symbolTable["NOT"].second;
                    binString+=symbolTable[sections[2]].second;
                    binString+=symbolTable[sections[3]].second;
                    binString+="111111";
                    QTableWidgetItem* item=new QTableWidgetItem();
                    int res=binString.toInt(nullptr,2);
                    item->setText(QString::number(res,16).toUpper());
                    item->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
                    ui->memoryTable->setItem(++memoryCellNumber,2,item);
                    }
                }
                else if(sections[1]=="LD"){
                    if(sections.size()!=4){
                    QMessageBox message;
                    message.setText("The format of the LD command is incorrect.");
                    message.setWindowIcon(QIcon(":/fonts & pictures/images/icon.png"));
                    message.setStyleSheet("color: rgb(255, 255, 255); background-color: rgb(25,35,75);");
                    message.setIcon(QMessageBox::Critical);
                    message.exec();
                    i--;
                    break;
                    }
                    else{
                    binString+=symbolTable["LD"].second;
                    binString+=symbolTable[sections[2]].second;
                    if(symbolTable.find(sections[3])!=symbolTable.end()){
                       binString+=getTwosComplement(symbolTable[sections[3]].second.toInt(),9);
                    }
                    else{
                        int res=0;
                        stringToInt(sections[3],res);
                        binString+=getTwosComplement(res,9);
                    }
                    QTableWidgetItem* item=new QTableWidgetItem();
                    int res=binString.toInt(nullptr,2);
                    item->setText(QString::number(res,16).toUpper());
                    item->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
                    ui->memoryTable->setItem(++memoryCellNumber,2,item);
                    }
                }
                else if(sections[1]=="LDI"){
                    if(sections.size()!=4){
                    QMessageBox message;
                    message.setText("The format of the LDI command is incorrect.");
                    message.setWindowIcon(QIcon(":/fonts & pictures/images/icon.png"));
                    message.setStyleSheet("color: rgb(255, 255, 255); background-color: rgb(25,35,75);");
                    message.setIcon(QMessageBox::Critical);
                    message.exec();
                    i--;
                    break;
                    }
                    else{
                    binString+=symbolTable["LDI"].second;
                    binString+=symbolTable[sections[2]].second;
                    if(symbolTable.find(sections[3])!=symbolTable.end()){
                        binString+=getTwosComplement(symbolTable[sections[3]].second.toInt(),9);
                    }
                    else{
                        int res=0;
                        stringToInt(sections[3],res);
                        binString+=getTwosComplement(res,9);
                    }
                    QTableWidgetItem* item=new QTableWidgetItem();
                    int res=binString.toInt(nullptr,2);
                    item->setText(QString::number(res,16).toUpper());
                    item->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
                    ui->memoryTable->setItem(++memoryCellNumber,2,item);
                    }
                }
                else if(sections[1]=="LDR"){
                    if(sections.size()!=5){
                    QMessageBox message;
                    message.setText("The format of the LDR command is incorrect.");
                    message.setWindowIcon(QIcon(":/fonts & pictures/images/icon.png"));
                    message.setStyleSheet("color: rgb(255, 255, 255); background-color: rgb(25,35,75);");
                    message.setIcon(QMessageBox::Critical);
                    message.exec();
                    i--;
                    break;
                    }
                    else{
                    binString+=symbolTable["LDR"].second;
                    binString+=symbolTable[sections[2]].second;
                    binString+=symbolTable[sections[3]].second;
                    if(symbolTable.find(sections[4])!=symbolTable.end()){
                        binString+=getTwosComplement(symbolTable[sections[4]].second.toInt(),6);
                    }
                    else{
                        int res=0;
                        stringToInt(sections[4],res);
                        binString+=getTwosComplement(res,6);
                    }
                    QTableWidgetItem* item=new QTableWidgetItem();
                    int res=binString.toInt(nullptr,2);
                    item->setText(QString::number(res,16).toUpper());
                    item->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
                    ui->memoryTable->setItem(++memoryCellNumber,2,item);
                    }
                }
                else if(sections[1]=="LEA"){
                    if(sections.size()!=4){
                    QMessageBox message;
                    message.setText("The format of the LEA command is incorrect.");
                    message.setWindowIcon(QIcon(":/fonts & pictures/images/icon.png"));
                    message.setStyleSheet("color: rgb(255, 255, 255); background-color: rgb(25,35,75);");
                    message.setIcon(QMessageBox::Critical);
                    message.exec();
                    i--;
                    break;
                    }
                    else{
                    binString+=symbolTable["LEA"].second;
                    binString+=symbolTable[sections[2]].second;
                    if(symbolTable.find(sections[3])!=symbolTable.end()){
                        binString+=getTwosComplement(symbolTable[sections[3]].second.toInt(),9);
                    }
                    else{
                        int res=0;
                        stringToInt(sections[3],res);
                        binString+=getTwosComplement(res,9);
                    }
                    QTableWidgetItem* item=new QTableWidgetItem();
                    int res=binString.toInt(nullptr,2);
                    item->setText(QString::number(res,16).toUpper());
                    item->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
                    ui->memoryTable->setItem(++memoryCellNumber,2,item);
                    }
                }
                else if(sections[1]=="ST"){
                    if(sections.size()!=4){
                    QMessageBox message;
                    message.setText("The format of the ST command is incorrect.");
                    message.setWindowIcon(QIcon(":/fonts & pictures/images/icon.png"));
                    message.setStyleSheet("color: rgb(255, 255, 255); background-color: rgb(25,35,75);");
                    message.setIcon(QMessageBox::Critical);
                    message.exec();
                    i--;
                    break;
                    }
                    else{
                    binString+=symbolTable["ST"].second;
                    binString+=symbolTable[sections[2]].second;
                    if(symbolTable.find(sections[3])!=symbolTable.end()){
                         binString+=getTwosComplement(symbolTable[sections[3]].second.toInt(),9);
                    }
                    else{
                        int res=0;
                        stringToInt(sections[3],res);
                        binString+=getTwosComplement(res,9);
                    }
                    QTableWidgetItem* item=new QTableWidgetItem();
                    int res=binString.toInt(nullptr,2);
                    item->setText(QString::number(res,16).toUpper());
                    item->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
                    ui->memoryTable->setItem(++memoryCellNumber,2,item);
                    }
                }
                else if(sections[1]=="STI"){
                    if(sections.size()!=4){
                    QMessageBox message;
                    message.setText("The format of the STI command is incorrect.");
                    message.setWindowIcon(QIcon(":/fonts & pictures/images/icon.png"));
                    message.setStyleSheet("color: rgb(255, 255, 255); background-color: rgb(25,35,75);");
                    message.setIcon(QMessageBox::Critical);
                    message.exec();
                    i--;
                    break;
                    }
                    else{
                    binString+=symbolTable["STI"].second;
                    binString+=symbolTable[sections[2]].second;
                    if(symbolTable.find(sections[3])!=symbolTable.end()){
                        binString+=getTwosComplement(symbolTable[sections[3]].second.toInt(),9);
                    }
                    else{
                        int res=0;
                        stringToInt(sections[3],res);
                        binString+=getTwosComplement(res,9);
                    }
                    QTableWidgetItem* item=new QTableWidgetItem();
                    int res=binString.toInt(nullptr,2);
                    item->setText(QString::number(res,16).toUpper());
                    item->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
                    ui->memoryTable->setItem(++memoryCellNumber,2,item);
                    }
                }
                else if(sections[1]=="STR"){
                    if(sections.size()!=5){
                    QMessageBox message;
                    message.setText("The format of the STR command is incorrect.");
                    message.setWindowIcon(QIcon(":/fonts & pictures/images/icon.png"));
                    message.setStyleSheet("color: rgb(255, 255, 255); background-color: rgb(25,35,75);");
                    message.setIcon(QMessageBox::Critical);
                    message.exec();
                    i--;
                    break;
                    }
                    else{
                    binString+=symbolTable["STR"].second;
                    binString+=symbolTable[sections[2]].second;
                    binString+=symbolTable[sections[3]].second;
                    if(symbolTable.find(sections[4])!=symbolTable.end()){
                        binString+=getTwosComplement(symbolTable[sections[4]].second.toInt(),6);
                    }
                    else{
                        int res=0;
                        stringToInt(sections[4],res);
                        binString+=getTwosComplement(res,6);
                    }
                    QTableWidgetItem* item=new QTableWidgetItem();
                    int res=binString.toInt(nullptr,2);
                    item->setText(QString::number(res,16).toUpper());
                    item->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
                    ui->memoryTable->setItem(++memoryCellNumber,2,item);
                    }
                }
                else if(sections[1].contains("BR")){
                    if(sections.size()!=3){
                    QMessageBox message;
                    message.setText("The format of the BR command is incorrect.");
                    message.setWindowIcon(QIcon(":/fonts & pictures/images/icon.png"));
                    message.setStyleSheet("color: rgb(255, 255, 255); background-color: rgb(25,35,75);");
                    message.setIcon(QMessageBox::Critical);
                    message.exec();
                    i--;
                    break;
                    }
                    else{
                    binString+=symbolTable[sections[1]].second;
                    if(symbolTable.find(sections[2])!=symbolTable.end()){
                        binString+=getTwosComplement(symbolTable[sections[2]].second.toInt(),9);
                    }
                    else{
                        int res=0;
                        stringToInt(sections[2],res);
                        binString+=getTwosComplement(res,9);
                    }
                    QTableWidgetItem* item=new QTableWidgetItem();
                    int res=binString.toInt(nullptr,2);
                    item->setText(QString::number(res,16).toUpper());
                    item->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
                    ui->memoryTable->setItem(++memoryCellNumber,2,item);
                    }
                }
                else if(sections[1]=="JMP"){
                    if(sections.size()!=3){
                    QMessageBox message;
                    message.setText("The format of the JMP command is incorrect.");
                    message.setWindowIcon(QIcon(":/fonts & pictures/images/icon.png"));
                    message.setStyleSheet("color: rgb(255, 255, 255); background-color: rgb(25,35,75);");
                    message.setIcon(QMessageBox::Critical);
                    message.exec();
                    i--;
                    break;
                    }
                    else{
                    binString+=symbolTable["JMP"].second;
                    binString+=symbolTable[sections[2]].second;
                    binString+="000000";
                    QTableWidgetItem* item=new QTableWidgetItem();
                    int res=binString.toInt(nullptr,2);
                    item->setText(QString::number(res,16).toUpper());
                    item->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
                    ui->memoryTable->setItem(++memoryCellNumber,2,item);
                    }
                }
                else if(sections[1]=="RET"){
                    if(sections.size()!=2){
                    QMessageBox message;
                    message.setText("The format of the RET command is incorrect.");
                    message.setWindowIcon(QIcon(":/fonts & pictures/images/icon.png"));
                    message.setStyleSheet("color: rgb(255, 255, 255); background-color: rgb(25,35,75);");
                    message.setIcon(QMessageBox::Critical);
                    message.exec();
                    i--;
                    break;
                    }
                    else{
                    binString+=symbolTable["RET"].second;
                    QTableWidgetItem* item=new QTableWidgetItem();
                    int res=binString.toInt(nullptr,2);
                    item->setText(QString::number(res,16).toUpper());
                    item->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
                    ui->memoryTable->setItem(++memoryCellNumber,2,item);
                    }
                }
                else if(sections[1]=="JSR"){
                    if(sections.size()!=3){
                    QMessageBox message;
                    message.setText("The format of the JSR command is incorrect.");
                    message.setWindowIcon(QIcon(":/fonts & pictures/images/icon.png"));
                    message.setStyleSheet("color: rgb(255, 255, 255); background-color: rgb(25,35,75);");
                    message.setIcon(QMessageBox::Critical);
                    message.exec();
                    i--;
                    break;
                    }
                    else{
                    binString+=symbolTable["JSR"].second;
                    if(symbolTable.find(sections[2])!=symbolTable.end()){
                        binString+=getTwosComplement(symbolTable[sections[2]].second.toInt(),11);
                    }
                    else{
                        int res=0;
                        stringToInt(sections[2],res);
                        binString+=getTwosComplement(res,11);
                    }
                    QTableWidgetItem* item=new QTableWidgetItem();
                    int res=binString.toInt(nullptr,2);
                    item->setText(QString::number(res,16).toUpper());
                    item->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
                    ui->memoryTable->setItem(++memoryCellNumber,2,item);
                    }
                }
                else if(sections[1]=="JSRR"){
                    if(sections.size()!=3){
                    QMessageBox message;
                    message.setText("The format of the JSRR command is incorrect.");
                    message.setWindowIcon(QIcon(":/fonts & pictures/images/icon.png"));
                    message.setStyleSheet("color: rgb(255, 255, 255); background-color: rgb(25,35,75);");
                    message.setIcon(QMessageBox::Critical);
                    message.exec();
                    i--;
                    break;
                    }
                    else{
                    binString+=symbolTable["JSRR"].second;
                    binString+=symbolTable[sections[2]].second;
                    binString+="000000";
                    QTableWidgetItem* item=new QTableWidgetItem();
                    int res=binString.toInt(nullptr,2);
                    item->setText(QString::number(res,16).toUpper());
                    item->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
                    ui->memoryTable->setItem(++memoryCellNumber,2,item);
                    }
                }
                else if(sections[1]=="HALT"){
                    if(sections.size()!=2){
                    QMessageBox message;
                    message.setText("The format of the HALT command is incorrect.");
                    message.setWindowIcon(QIcon(":/fonts & pictures/images/icon.png"));
                    message.setStyleSheet("color: rgb(255, 255, 255); background-color: rgb(25,35,75);");
                    message.setIcon(QMessageBox::Critical);
                    message.exec();
                    i--;
                    break;
                    }
                    else{
                    binString+=symbolTable["HALT"].second;
                    QTableWidgetItem* item=new QTableWidgetItem();
                    int res=binString.toInt(nullptr,2);
                    item->setText(QString::number(res,16).toUpper());
                    item->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
                    ui->memoryTable->setItem(++memoryCellNumber,2,item);
                    }
                }
                else if(sections[1]=="DEC"){
                    binString+=getTwosComplement(symbolTable[sections[0]].second.toInt(),16);
                    QTableWidgetItem* item=new QTableWidgetItem();
                    int res=binString.toInt(nullptr,2);
                    item->setText(QString::number(res,16).toUpper());
                    item->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
                    ui->memoryTable->setItem(++memoryCellNumber,2,item);
                }
                else if(sections[1]=="HEX"){
                    binString+=getTwosComplement(symbolTable[sections[0]].second.toInt(nullptr,16),16);
                    QTableWidgetItem* item=new QTableWidgetItem();
                    int res=binString.toInt(nullptr,2);
                    item->setText(QString::number(res,16).toUpper());
                    item->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
                    ui->memoryTable->setItem(++memoryCellNumber,2,item);
                }
            }
        }
    }
}







