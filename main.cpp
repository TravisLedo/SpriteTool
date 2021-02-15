#include "mainwindow.h"
#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setApplicationName("your title");
    QFile file("style.qss");
    file.open(QFile::ReadWrite);
    QString stylesheet = file.readAll();
    a.setStyleSheet(stylesheet);

    MainWindow w;
    QIcon icon(":/images/images/app_icon.ico");
    w.setWindowIcon(icon);
    w.showMaximized();
    return a.exec();
}
