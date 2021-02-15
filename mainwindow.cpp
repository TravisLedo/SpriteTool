#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "imagefile.h"
#include <QtCore>
#include <QtGui>
#include <QListWidget>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QIcon>
#include <QDebug>
#include <QResizeEvent>
#include <QPixmap>
#include <QMessageBox>
#include <QStyleFactory>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

setup();

}

void MainWindow::setup()
{
    ui->setupUi(this);
    setupMergeContent();//Setup everything needed for the Merge page
    setupSplitContent();

    // Keep consistant style on all machines
       qApp->setStyle(QStyleFactory::create("Fusion"));
       // increase font size for better reading
       QFont defaultFont = QApplication::font();
       defaultFont.setPointSize(defaultFont.pointSize()+2);
       qApp->setFont(defaultFont);
       // modify palette to dark
       QPalette darkPalette;
       darkPalette.setColor(QPalette::Window,QColor(53,53,53));
       darkPalette.setColor(QPalette::WindowText,Qt::white);
       darkPalette.setColor(QPalette::Disabled,QPalette::WindowText,QColor(127,127,127));
       darkPalette.setColor(QPalette::Base,QColor(42,42,42));
       darkPalette.setColor(QPalette::AlternateBase,QColor(66,66,66));
       darkPalette.setColor(QPalette::ToolTipBase,Qt::white);
       darkPalette.setColor(QPalette::ToolTipText,Qt::white);
       darkPalette.setColor(QPalette::Text,Qt::white);
       darkPalette.setColor(QPalette::Disabled,QPalette::Text,QColor(127,127,127));
       darkPalette.setColor(QPalette::Dark,QColor(35,35,35));
       darkPalette.setColor(QPalette::Shadow,QColor(20,20,20));
       darkPalette.setColor(QPalette::Button,QColor(53,53,53));
       darkPalette.setColor(QPalette::ButtonText,Qt::white);
       darkPalette.setColor(QPalette::Disabled,QPalette::ButtonText,QColor(127,127,127));
       darkPalette.setColor(QPalette::BrightText,Qt::red);
       darkPalette.setColor(QPalette::Link,QColor(42,130,218));
       darkPalette.setColor(QPalette::Highlight,QColor(42,130,218));
       darkPalette.setColor(QPalette::Disabled,QPalette::Highlight,QColor(80,80,80));
       darkPalette.setColor(QPalette::HighlightedText,Qt::white);
       darkPalette.setColor(QPalette::Disabled,QPalette::HighlightedText,QColor(127,127,127));
       qApp->setPalette(darkPalette);

       ui->hyperlinkText->setText("<a href=\"https://github.com/TravisLedo/\">Sprite Tool 1.0.0</a>");
       ui->hyperlinkText->setTextFormat(Qt::RichText);
       ui->hyperlinkText->setTextInteractionFlags(Qt::TextBrowserInteraction);
       ui->hyperlinkText->setOpenExternalLinks(true);
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::resizeEvent(QResizeEvent* event)
{
   QMainWindow::resizeEvent(event);
   on_mergeFileListWidget_currentRowChanged(ui->mergeFileListWidget->currentRow());//make preview image scale with resize
}



