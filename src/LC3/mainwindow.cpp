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
#include <algorithm>
using namespace std;
QString getTwosComplement(int num, int bits);
QString signExtend(QString binString,int newLength);

QString PC="0",IR="0",MAR="0",MDR="0",CC="-";
vector<QString>registerFile({"0","0","0","0","0","0","0","0"});
vector<bool>decodeResult(16);
vector<bool> ccVec(3);

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

    //initialize PC
    PC=getTwosComplement(12288,16);
    fill(decodeResult.begin(),decodeResult.end(),false);
    fill(ccVec.begin(),ccVec.end(),false);

    for(int i=0;i<8;i++){
        registerFile[i]=signExtend(registerFile[i],16);
    }
    showContentOfRegisters();
}

MainWindow::~MainWindow()
{
    delete ui;
}


int twosComplementToInt(const QString& binaryStr) {
    int n = binaryStr.size();
    int value = 0;

    // Convert binary string to integer
    for (int i = 0; i < n; ++i) {
        if (binaryStr[i] == '1') {
            value += std::pow(2, n - 1 - i);
        }
    }

    // Check if the number is negative
    if (binaryStr[0] == '1') {
        value -= std::pow(2, n);
    }

    return value;
}

void MainWindow::showContentOfRegisters()
{
    int tmp;

    ui->CC->setText(CC);

    tmp=registerFile[0].toInt(nullptr,2);
    ui->R0_Hex->setText(QString::number(tmp,16));
    ui->R0_Dec->setText(QString::number(twosComplementToInt(registerFile[0])));

    tmp=registerFile[1].toInt(nullptr,2);
    ui->R1Hex->setText(QString::number(tmp,16));
    ui->R1Dec->setText(QString::number(twosComplementToInt(registerFile[1])));

    tmp=registerFile[2].toInt(nullptr,2);
    ui->R2Hex->setText(QString::number(tmp,16));
    ui->R2Dec->setText(QString::number(twosComplementToInt(registerFile[2])));

    tmp=registerFile[3].toInt(nullptr,2);
    ui->R3Hex->setText(QString::number(tmp,16));
    ui->R3Dec->setText(QString::number(twosComplementToInt(registerFile[3])));

    tmp=registerFile[4].toInt(nullptr,2);
    ui->R4Hex->setText(QString::number(tmp,16));
    ui->R4Dec->setText(QString::number(twosComplementToInt(registerFile[4])));

    tmp=registerFile[5].toInt(nullptr,2);
    ui->R5Hex->setText(QString::number(tmp,16));
    ui->R5Dec->setText(QString::number(twosComplementToInt(registerFile[5])));

    tmp=registerFile[6].toInt(nullptr,2);
    ui->R6Hex->setText(QString::number(tmp,16));
    ui->R6Dec->setText(QString::number(twosComplementToInt(registerFile[6])));

    tmp=registerFile[7].toInt(nullptr,2);
    ui->R7Hex->setText(QString::number(tmp,16));
    ui->R7Dec->setText(QString::number(twosComplementToInt(registerFile[7])));

    tmp=PC.toInt(nullptr,2);
    ui->PCHex->setText(QString::number(tmp,16));
    ui->PCDec->setText(QString::number(twosComplementToInt(PC)));

    reverse(IR.begin(),IR.end());
    tmp=IR.toInt(nullptr,2);
    ui->IRHex->setText(QString::number(tmp,16));
    ui->IRDec->setText(QString::number(twosComplementToInt(IR)));
    reverse(IR.begin(),IR.end());

    tmp=MAR.toInt(nullptr,2);
    ui->MARHex->setText(QString::number(tmp,16));
    ui->MARDec->setText(QString::number(twosComplementToInt(MAR)));

    tmp=MDR.toInt(nullptr,2);
    ui->MDRHex->setText(QString::number(tmp,16));
    ui->MDRDec->setText(QString::number(twosComplementToInt(MDR)));

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
        res=s.toInt(nullptr,2); //error
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
                    symbolTable.insert({sections[0],{0,(QString::number(memoryCellNumber-1))}});
                    symbolTable.insert({sections[0]+",",{0,(QString::number(memoryCellNumber-1))}});
                }
                else if(symbolTable.find(sections[0])==symbolTable.end()
                           && (sections[1]=="DEC" || sections[1]=="HEX")){
                    sections[0].remove(',');
                    if(sections[1]=="DEC"){
//                        int res=sections[2].toInt(nullptr,10);
//                        symbolTable.insert({sections[0],{0,QString::number(res)}});
//                        symbolTable.insert({sections[0]+",",{0,QString::number(res)}});
                        symbolTable.insert({sections[0],{0,(QString::number(memoryCellNumber-1))}});
                        symbolTable.insert({sections[0]+",",{0,(QString::number(memoryCellNumber-1))}});
                    }
                    else if(sections[0]=="HEX"){
//                        int res=sections[2].toInt(nullptr,16);
//                        symbolTable.insert({sections[0],{0,QString::number(res)}});
//                        symbolTable.insert({sections[0]+",",{0,QString::number(res)}});
                        symbolTable.insert({sections[0],{0,(QString::number(memoryCellNumber-1))}});
                        symbolTable.insert({sections[0]+",",{0,(QString::number(memoryCellNumber-1))}});
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
//                        binString+=getTwosComplement(symbolTable[sections[2]].second.toInt(),9);
                            binString+=getTwosComplement(symbolTable[sections[2]].second.toInt()-(memoryCellNumber+1),9);
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
                       // binString+=getTwosComplement(symbolTable[sections[2]].second.toInt(),9);
                        binString+=getTwosComplement(symbolTable[sections[2]].second.toInt()-(memoryCellNumber+1),9);

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
                    //binString+=getTwosComplement(symbolTable[sections[2]].second.toInt(),9);
                        binString+=getTwosComplement(symbolTable[sections[2]].second.toInt()-(memoryCellNumber+1),9);

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
                    //binString+=getTwosComplement(symbolTable[sections[2]].second.toInt(),9);
                    binString+=getTwosComplement(symbolTable[sections[2]].second.toInt()-(memoryCellNumber+1),9);

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
                    //binString+=getTwosComplement(symbolTable[sections[2]].second.toInt(),9);
                    binString+=getTwosComplement(symbolTable[sections[2]].second.toInt()-(memoryCellNumber+1),9);

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
                    //binString+=getTwosComplement(symbolTable[sections[1]].second.toInt(),9);
                    binString+=getTwosComplement(symbolTable[sections[1]].second.toInt()-(memoryCellNumber+1),9);

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
                    //binString+=getTwosComplement(symbolTable[sections[1]].second.toInt(),11);
                     binString+=getTwosComplement(symbolTable[sections[1]].second.toInt()-(memoryCellNumber+1),11);
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
                       //binString+=getTwosComplement(symbolTable[sections[3]].second.toInt(),9);
                        binString+=getTwosComplement(symbolTable[sections[3]].second.toInt()-(memoryCellNumber+1),9);

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
                        //binString+=getTwosComplement(symbolTable[sections[3]].second.toInt(),9);
                        binString+=getTwosComplement(symbolTable[sections[3]].second.toInt()-(memoryCellNumber+1),9);
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
//                        binString+=getTwosComplement(symbolTable[sections[3]].second.toInt(),9);
                        binString+=getTwosComplement(symbolTable[sections[3]].second.toInt()-(memoryCellNumber+1),9);
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
                         //binString+=getTwosComplement(symbolTable[sections[3]].second.toInt(),9);
                        binString+=getTwosComplement(symbolTable[sections[3]].second.toInt()-(memoryCellNumber+1),9);
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
                       // binString+=getTwosComplement(symbolTable[sections[3]].second.toInt(),9);
                        binString+=getTwosComplement(symbolTable[sections[3]].second.toInt()-(memoryCellNumber+1),9);
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
                        //binString+=getTwosComplement(symbolTable[sections[2]].second.toInt(),9);
                        binString+=getTwosComplement(symbolTable[sections[2]].second.toInt()-(memoryCellNumber+1),9);
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
                        //binString+=getTwosComplement(symbolTable[sections[2]].second.toInt(),11);
                        binString+=getTwosComplement(symbolTable[sections[2]].second.toInt()-(memoryCellNumber+1),11);
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
                    binString+=getTwosComplement(/*symbolTable[sections[0]].second*/sections[2].toInt(),16);
                    QTableWidgetItem* item=new QTableWidgetItem();
                    //int res=binString.toInt(nullptr,2); //posibility of error
                    int res=twosComplementToInt(binString);
                    item->setText(QString::number(res,16).toUpper());
                    item->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
                    ui->memoryTable->setItem(++memoryCellNumber,2,item);
                }
                else if(sections[1]=="HEX"){
                    binString+=getTwosComplement(/*symbolTable[sections[0]].second*/sections[2].toInt(nullptr,16),16);
                    QTableWidgetItem* item=new QTableWidgetItem();
                    int res=binString.toInt(nullptr,2);
                    item->setText(QString::number(res,16).toUpper());
                    item->setData(Qt::TextAlignmentRole,Qt::AlignCenter);
                    ui->memoryTable->setItem(++memoryCellNumber,2,item);
                }
            }
        }
    }
    ui->fetchButton->setEnabled(true);
    ui->decodeButton->setEnabled(true);
    ui->executeButton->setEnabled(true);
}





void MainWindow::on_fetchButton_clicked()
{
    //clk1
    // MAR<-PC
    MAR=PC;
    //PC<-PC+1
    int PCValue= PC.toInt(nullptr,2);
    PCValue++;
    PC=getTwosComplement(PCValue,16);
    //clk2
    //MDR<-M[MAR]
    int MARValue=MAR.toInt(nullptr,2);
    QTableWidgetItem*item=ui->memoryTable->item(MARValue,2);
    QString M_MAR=item->text();
    int M_MARValue=M_MAR.toInt(nullptr,16);
    MDR=getTwosComplement(M_MARValue,16);
    //clk3
    //IR<-MDR
    IR=MDR;
    reverse(IR.begin(),IR.end());
    showContentOfRegisters();
}



void MainWindow::on_decodeButton_clicked()
{
    //clk4
    fill(decodeResult.begin(),decodeResult.end(),false);

    QString opCode="";

    opCode+=IR[15];
    opCode+=IR[14];
    opCode+=IR[13];
    opCode+=IR[12];
    int opCodeValue=opCode.toInt(nullptr,2);

    decodeResult[opCodeValue]=true;
}


void MainWindow::on_executeButton_clicked()
{
    //after clk4
    if(decodeResult[0]){
        // BR
        BR();
    }
    else if(decodeResult[1]){
        // ADD
        ADD();
    }
    else if(decodeResult[2]){
        // LD
        LD();
    }
    else if(decodeResult[3]){
        // ST
        ST();
    }
    else if(decodeResult[4]){
        if(IR[11]=='1'){
         // JSR
            JSR();
        }
        else{
            // JSRR
            JSRR();
        }
    }
    else if(decodeResult[5]){
        // AND
        AND();
    }
    else if(decodeResult[6]){
        //LDR
        LDR();
    }
    else if(decodeResult[7]){
        //STR
        STR();
    }
    else if(decodeResult[8]){
        //RTI
    }
    else if(decodeResult[9]){
        // NOT
        NOT();
    }
    else if(decodeResult[10]){
        // LDI
        LDI();
    }
    else if(decodeResult[11]){
        // STI
        STI();
    }
    else if(decodeResult[12]){
        // JMP
        if(IR=="0000001110000011")
            RET();
        else JMP();
    }
    else if(decodeResult[13]){
        // RESERVED
    }
    else if(decodeResult[14]){
        // LEA
        LEA();
    }
    else if(decodeResult[15]){
        // TRAP
        if(IR=="1010010000001111")
            HALT();
    }
}


void MainWindow::ADD(){
    int dst,src1,src2;
    QString value1,value2;
    QString s="";

    s+=IR[11];
    s+=IR[10];
    s+=IR[9];

    dst=s.toInt(nullptr,2);
    s="";

    s+=IR[8];
    s+=IR[7];
    s+=IR[6];

    src1=s.toInt(nullptr,2);
    value1=registerFile[src1];
    if(IR[5]=='0'){
        s="";

        s+=IR[2];
        s+=IR[1];
        s+=IR[0];

        src2=s.toInt(nullptr,2);
        value2=registerFile[src2];
    }
    else{
        s="";

        s+=IR[4];
        s+=IR[3];
        s+=IR[2];
        s+=IR[1];
        s+=IR[0];

        value2=signExtend(s,16);
    }

    QChar carry='0';
    for(int i=15;i>=0;i--){
        if(value1[i]=='0' && value2[i]=='0'){
            registerFile[dst][i]=carry;
            carry='0';
        }
        else if((value1[i]=='0' && value2[i]=='1')||(value1[i]=='1' && value2[i]=='0')){
            if(carry=='1')
                registerFile[dst][i]='0';
            else registerFile[dst][i]='1';
        }
        else if(value1[i]=='1'&&value2[i]=='1'){
            registerFile[dst][i]=carry;
            carry='1';
        }
    }
    bool b=true;
    for(int i=0;i<16;i++){
        if(registerFile[dst][i]=='1'){
            b=false;
            break;
        }
    }
    if(b){
        CC="Z";
        fill(ccVec.begin(),ccVec.end(),false);
        ccVec[1]=true;
    }
    else{
        if(registerFile[dst][0]=='0'){
            CC="P";
            fill(ccVec.begin(),ccVec.end(),false);
            ccVec[2]=true;
        }
        else {
            CC="N";
            fill(ccVec.begin(),ccVec.end(),false);
            ccVec[0]=true;
        }
    }

    showContentOfRegisters();
}

void MainWindow::AND()
{
    int dst,src1,src2;
    QString value1,value2;
    QString s="";

    s+=IR[11];
    s+=IR[10];
    s+=IR[9];

    dst=s.toInt(nullptr,2);
    s="";

    s+=IR[8];
    s+=IR[7];
    s+=IR[6];

    src1=s.toInt(nullptr,2);
    value1=registerFile[src1];
    if(IR[5]=='0'){
        s="";

        s+=IR[2];
        s+=IR[1];
        s+=IR[0];

        src2=s.toInt(nullptr,2);
        value2=registerFile[src2];
    }
    else{
        s="";

        s+=IR[4];
        s+=IR[3];
        s+=IR[2];
        s+=IR[1];
        s+=IR[0];

        value2=signExtend(s,16);
    }


    for(int i=15;i>=0;i--){
        if(value1[i]=='1' && value2[i]=='1')
            registerFile[dst][i]='1';
        else registerFile[dst][i]='0';
    }

    bool b=true;
    for(int i=0;i<16;i++){
        if(registerFile[dst][i]=='1'){
            b=false;
            break;
        }
    }
    if(b){
        CC="Z";
        fill(ccVec.begin(),ccVec.end(),false);
        ccVec[1]=true;
    }
    else{
        if(registerFile[dst][0]=='0'){
            CC="P";
            fill(ccVec.begin(),ccVec.end(),false);
            ccVec[2]=true;
        }
        else {
            CC="N";
            fill(ccVec.begin(),ccVec.end(),false);
            ccVec[0]=true;
        }
    }

    showContentOfRegisters();
}

void MainWindow::NOT()
{
    int dst,src;
    QString value;
    QString s="";

    s+=IR[11];
    s+=IR[10];
    s+=IR[9];

    dst=s.toInt(nullptr,2);
    s="";

    s+=IR[8];
    s+=IR[7];
    s+=IR[6];

    src=s.toInt(nullptr,2);
    value=registerFile[src];

    for(int i=15;i>=0;i--){
        if(value[i]=='0')
            registerFile[dst][i]='1';
        else registerFile[dst][i]='0';
    }

    bool b=true;
    for(int i=0;i<16;i++){
        if(registerFile[dst][i]=='1'){
            b=false;
            break;
        }
    }
    if(b){
        CC="Z";
        fill(ccVec.begin(),ccVec.end(),false);
        ccVec[1]=true;
    }
    else{
        if(registerFile[dst][0]=='0'){
            CC="P";
            fill(ccVec.begin(),ccVec.end(),false);
            ccVec[2]=true;
        }
        else {
            CC="N";
            fill(ccVec.begin(),ccVec.end(),false);
            ccVec[0]=true;
        }
    }

    showContentOfRegisters();
}

QString Adder(QString value1,QString value2){
    QString res="0000000000000000";

    QChar carry='0';
    for(int i=15;i>=0;i--){
        if(value1[i]=='0' && value2[i]=='0'){
            res[i]=carry;
            carry='0';
        }
        else if((value1[i]=='0' && value2[i]=='1')||(value1[i]=='1' && value2[i]=='0')){
            if(carry=='1')
                res[i]='0';
            else res[i]='1';
        }
        else if(value1[i]=='1'&&value2[i]=='1'){
            res[i]=carry;
            carry='1';
        }
    }
    return res;
}

void MainWindow::LD()
{
    int dst;
    QString s="";
    s+=IR[11];
    s+=IR[10];
    s+=IR[9];

    dst=s.toInt(nullptr,2);

    s="";
    s+=IR[8];
    s+=IR[7];
    s+=IR[6];
    s+=IR[5];
    s+=IR[4];
    s+=IR[3];
    s+=IR[2];
    s+=IR[1];
    s+=IR[0];

    s=signExtend(s,16);
    MAR=Adder(PC,s); //s == offset
    //MDR<-M[MAR]
    int MARValue=MAR.toInt(nullptr,2);
    QTableWidgetItem*item=ui->memoryTable->item(MARValue,2);
    QString M_MAR=item->text();
    int M_MARValue=M_MAR.toInt(nullptr,16);
    MDR=getTwosComplement(M_MARValue,16);

    // DR<-MDR
    registerFile[dst]=MDR;

    bool b=true;
    for(int i=0;i<16;i++){
        if(registerFile[dst][i]=='1'){
            b=false;
            break;
        }
    }
    if(b){
        CC="Z";
        fill(ccVec.begin(),ccVec.end(),false);
        ccVec[1]=true;
    }
    else{
        if(registerFile[dst][0]=='0'){
            CC="P";
            fill(ccVec.begin(),ccVec.end(),false);
            ccVec[2]=true;
        }
        else {
            CC="N";
            fill(ccVec.begin(),ccVec.end(),false);
            ccVec[0]=true;
        }
    }


    showContentOfRegisters();
}

void MainWindow::LDI()
{
    int dst;
    QString s="";
    s+=IR[11];
    s+=IR[10];
    s+=IR[9];

    dst=s.toInt(nullptr,2);

    s="";
    s+=IR[8];
    s+=IR[7];
    s+=IR[6];
    s+=IR[5];
    s+=IR[4];
    s+=IR[3];
    s+=IR[2];
    s+=IR[1];
    s+=IR[0];

    s=signExtend(s,16);
    MAR=Adder(PC,s); //s == offset
    //MDR<-M[MAR]
    int MARValue=MAR.toInt(nullptr,2);
    QTableWidgetItem*item1=ui->memoryTable->item(MARValue,2);
    QString M_MAR=item1->text();
    int M_MARValue=M_MAR.toInt(nullptr,16);
    MDR=getTwosComplement(M_MARValue,16);

    showContentOfRegisters();

    //MAR<-MDR
    MAR=MDR;
    MARValue=MAR.toInt(nullptr,2);
    QTableWidgetItem*item2=ui->memoryTable->item(MARValue,2);
    M_MAR=item2->text();
    M_MARValue=M_MAR.toInt(nullptr,16);
    MDR=getTwosComplement(M_MARValue,16);

    //DR<-MDR
    registerFile[dst]=MDR;

    bool b=true;
    for(int i=0;i<16;i++){
        if(registerFile[dst][i]=='1'){
            b=false;
            break;
        }
    }
    if(b){
        CC="Z";
        fill(ccVec.begin(),ccVec.end(),false);
        ccVec[1]=true;
    }
    else{
        if(registerFile[dst][0]=='0'){
            CC="P";
            fill(ccVec.begin(),ccVec.end(),false);
            ccVec[2]=true;
        }
        else {
            CC="N";
            fill(ccVec.begin(),ccVec.end(),false);
            ccVec[0]=true;
        }
    }


    showContentOfRegisters();
}

void MainWindow::LDR()
{
    int dst,baseR;
    QString s="";
    s+=IR[11];
    s+=IR[10];
    s+=IR[9];

    dst=s.toInt(nullptr,2);

    s="";
    s+=IR[8];
    s+=IR[7];
    s+=IR[6];

    baseR=s.toInt(nullptr,2);

    s="";
    s+=IR[5];
    s+=IR[4];
    s+=IR[3];
    s+=IR[2];
    s+=IR[1];
    s+=IR[0];

    s=signExtend(s,16);

    MAR=Adder(registerFile[baseR],s); //s == offset
    //MDR<-M[MAR]
    int MARValue=MAR.toInt(nullptr,2);
    QTableWidgetItem*item1=ui->memoryTable->item(MARValue,2);
    QString M_MAR=item1->text();
    int M_MARValue=M_MAR.toInt(nullptr,16);
    MDR=getTwosComplement(M_MARValue,16);
    //DR<-MDR
    registerFile[dst]=MDR;

    bool b=true;
    for(int i=0;i<16;i++){
        if(registerFile[dst][i]=='1'){
            b=false;
            break;
        }
    }
    if(b){
        CC="Z";
        fill(ccVec.begin(),ccVec.end(),false);
        ccVec[1]=true;
    }
    else{
        if(registerFile[dst][0]=='0'){
            CC="P";
            fill(ccVec.begin(),ccVec.end(),false);
            ccVec[2]=true;
        }
        else {
            CC="N";
            fill(ccVec.begin(),ccVec.end(),false);
            ccVec[0]=true;
        }
    }


    showContentOfRegisters();
}

void MainWindow::LEA()
{
    int dst;
    QString s="";
    s+=IR[11];
    s+=IR[10];
    s+=IR[9];

    dst=s.toInt(nullptr,2);

    s="";
    s+=IR[8];
    s+=IR[7];
    s+=IR[6];
    s+=IR[5];
    s+=IR[4];
    s+=IR[3];
    s+=IR[2];
    s+=IR[1];
    s+=IR[0];

    s=signExtend(s,16);
    registerFile[dst]=Adder(PC,s);

    showContentOfRegisters();
}

void MainWindow::ST()
{
    int src;
    QString s="";
    s+=IR[11];
    s+=IR[10];
    s+=IR[9];

    src=s.toInt(nullptr,2);

    s="";
    s+=IR[8];
    s+=IR[7];
    s+=IR[6];
    s+=IR[5];
    s+=IR[4];
    s+=IR[3];
    s+=IR[2];
    s+=IR[1];
    s+=IR[0];

    s=signExtend(s,16);
    MAR=Adder(PC,s); //s == offset

    //MDR<-SR
    MDR=registerFile[src];
    //M[MAR]<-MDR
    int MARValue=MAR.toInt(nullptr,2);
    QTableWidgetItem*item=ui->memoryTable->item(MARValue,2);
    item->setText(QString::number(twosComplementToInt(MDR),16));

    showContentOfRegisters();
}

void MainWindow::STI()
{
    int src;
    QString s="";
    s+=IR[11];
    s+=IR[10];
    s+=IR[9];

    src=s.toInt(nullptr,2);

    s="";
    s+=IR[8];
    s+=IR[7];
    s+=IR[6];
    s+=IR[5];
    s+=IR[4];
    s+=IR[3];
    s+=IR[2];
    s+=IR[1];
    s+=IR[0];

    s=signExtend(s,16);
    MAR=Adder(PC,s); //s == offset
    //MDR<-M[MAR]
    int MARValue=MAR.toInt(nullptr,2);
    QTableWidgetItem*item1=ui->memoryTable->item(MARValue,2);
    QString M_MAR=item1->text();
    int M_MARValue=M_MAR.toInt(nullptr,16);
    MDR=getTwosComplement(M_MARValue,16);

    showContentOfRegisters();

    //MAR<-MDR
    MAR=MDR;

    //MDR<-SR
    MDR=registerFile[src];

    //M[MAR]<-MDR
    MARValue=MAR.toInt(nullptr,2);
    QTableWidgetItem*item2=ui->memoryTable->item(MARValue,2);
    item2->setText(QString::number(twosComplementToInt(MDR),16));

    showContentOfRegisters();
}

void MainWindow::STR()
{
    int src,baseR;
    QString s="";
    s+=IR[11];
    s+=IR[10];
    s+=IR[9];

    src=s.toInt(nullptr,2);

    s="";
    s+=IR[8];
    s+=IR[7];
    s+=IR[6];

    baseR=s.toInt(nullptr,2);

    s="";
    s+=IR[5];
    s+=IR[4];
    s+=IR[3];
    s+=IR[2];
    s+=IR[1];
    s+=IR[0];

    s=signExtend(s,16);

    MAR=Adder(registerFile[baseR],s); //s == offset

    // MDR<-SR
    MDR=registerFile[src];

    //M[MAR]<-MDR
    int MARValue=MAR.toInt(nullptr,2);
    QTableWidgetItem*item1=ui->memoryTable->item(MARValue,2);
    item1->setText(QString::number(twosComplementToInt(MDR),16));

    showContentOfRegisters();
}

void MainWindow::BR()
{
    bool n,z,p;
    n= IR[11]=='1'?true:false;
    z= IR[10]=='1'?true:false;
    p= IR[9]=='1'?true:false;

    QString s="";
    s+=IR[8];
    s+=IR[7];
    s+=IR[6];
    s+=IR[5];
    s+=IR[4];
    s+=IR[3];
    s+=IR[2];
    s+=IR[1];
    s+=IR[0];

    s=signExtend(s,16);

    if((ccVec[0]&& n)||(ccVec[1]&& z)||(ccVec[2]&& p)||(n==false && z==false && p==false)){
        PC=Adder(PC,s);
    }

    showContentOfRegisters();
}

void MainWindow::JMP()
{
    int baseR;
    QString s="";
    s+=IR[8];
    s+=IR[7];
    s+=IR[6];

    baseR=s.toInt(nullptr,2);

    PC=Adder(registerFile[baseR],signExtend("0",16));

    showContentOfRegisters();
}

void MainWindow::RET()
{
    PC=Adder(registerFile[7],signExtend("0",16));

    showContentOfRegisters();
}

void MainWindow::JSR()
{
    QString s="";
    s+=IR[10];
    s+=IR[9];
    s+=IR[8];
    s+=IR[7];
    s+=IR[6];
    s+=IR[5];
    s+=IR[4];
    s+=IR[3];
    s+=IR[2];
    s+=IR[1];
    s+=IR[0];

    s=signExtend(s,16);

    QString temp=PC;
    PC=Adder(PC,s);
    registerFile[7]=temp;

    showContentOfRegisters();
}

void MainWindow::JSRR()
{
    int baseR;
    QString s="";
    s+=IR[8];
    s+=IR[7];
    s+=IR[6];

    baseR=s.toInt(nullptr,2);

    QString temp=PC;
    PC=registerFile[baseR];
    registerFile[7]=temp;

    showContentOfRegisters();
}

void MainWindow::HALT()
{
    ui->fetchButton->setEnabled(false);
    ui->decodeButton->setEnabled(false);
    ui->executeButton->setEnabled(false);
    PC=getTwosComplement(12288,16);

    QMessageBox message;
    message.setText("The program has been successfully executed.");
    message.setWindowIcon(QIcon(":/fonts & pictures/images/icon.png"));
    message.setStyleSheet("color: rgb(255, 255, 255); background-color: rgb(25,35,75);");
    message.setIcon(QMessageBox::Information);
    message.exec();
}



