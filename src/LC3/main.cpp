#include "mainwindow.h"

#include <QApplication>
#include <QFont>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFont defaultFont("Ready to Party");
    a.setFont(defaultFont);

    MainWindow w;
    w.show();

    return a.exec();
}
