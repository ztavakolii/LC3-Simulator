#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextCharFormat>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void showContentOfRegisters();

    void on_codeEditor_currentCharFormatChanged(const QTextCharFormat &format);

    void on_assembleButton_clicked();

    void on_fetchButton_clicked();

    void on_decodeButton_clicked();

    void on_executeButton_clicked();

    void ADD();

    void AND();

    void NOT();

    void LD();

    void LDI();

    void LDR();

    void LEA();

    void ST();

    void STI();

    void STR();

    void BR();

    void JMP();

    void RET();

    void JSR();

    void JSRR();

    void HALT();


private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
