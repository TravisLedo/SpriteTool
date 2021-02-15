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
#include <QColor>
#include <QCursor>
#include <QPair>

void MainWindow::setupSplitContent()
{
    //timer and bool for animation
    splitAnimationtimer = new QTimer(this);
    connect(splitAnimationtimer, SIGNAL(timeout()), this, SLOT(splitTimerUpdate()));
    splitIsAnimating = false;
}

//Gets called every x miliseconds. used to animate sprite
void MainWindow::splitTimerUpdate()
{
    int startingFrame;
    int endingFrame;
    if(ui->splitPlayAllGroupsCheckBox->checkState())
    {
        startingFrame = 0;
        endingFrame = splicedImagesList->count()-1;
    }
    else{
        startingFrame = (ui->splitColumnSpinBox->value() * ui->splitPlayGroupSpinBox->value()) - ui->splitColumnSpinBox->value();
        endingFrame = (ui->splitColumnSpinBox->value() * ui->splitPlayGroupSpinBox->value())-1;
    }

    if(splitAnimationIndex<endingFrame)
    {
        splitAnimationIndex++;
        splitChangeNextAnimationImage();
    }
    else{
        splitAnimationIndex = startingFrame;
        splitChangeNextAnimationImage();
    }
}

void MainWindow::on_splitAddImageButtton_clicked()
{
    QFileDialog openFilesDialog;
    QString fileName = openFilesDialog.getOpenFileName(this, tr("Open File"),"/path/to/file",tr("PNG Files (*.png)")); //store list of path/filenames

    if(fileName.length()>1)
    {
    splitOriginalImage = QPixmap(fileName);
    splitUpdateSheetTable();
    }
    openFilesDialog.close();
}

void MainWindow::splitUpdateSheetTable()
{
    ui->splitTableWidget->setRowCount(1);
    ui->splitTableWidget->setColumnCount(1);

    QIcon splitImageIcon(splitOriginalImage);

    splitImageButton = new QPushButton();
    connect(splitImageButton, SIGNAL(clicked()),this, SLOT(splitPreviewImageClick()));
    splitImageButton->setIconSize(splitOriginalImage.size());
    splitImageButton->setIcon(splitImageIcon);
    splitImageButton->setStyleSheet("background-color: red; border: 1px ");
    splitImageButton->setFixedSize(splitOriginalImage.size());

    ui->splitTableWidget->setCellWidget(0, 0, splitImageButton);//put button into cell
    ui->splitTableWidget->horizontalHeader()->setDefaultSectionSize(splitImageButton->width());
    ui->splitTableWidget->verticalHeader()->setDefaultSectionSize(splitImageButton->height());

    splitDrawLines();
    splice_images();
}

void MainWindow::on_splitColumnSpinBox_valueChanged(int arg1)
{
    int startingFrame = (arg1 * ui->splitPlayGroupSpinBox->value()) - arg1;
    splitAnimationIndex = startingFrame;
    if(splicedImagesList->count()>0)
    {
    splitDrawLines();
    splice_images();
    }
}

void MainWindow::on_splitRowSpinBox_valueChanged(int arg1)
{
    ui->splitPlayGroupSpinBox->setMaximum(arg1);

    //Set max rows allowed in animation selection
    if(ui->splitPlayGroupSpinBox->value()>arg1)
    {
    ui->splitPlayGroupSpinBox->setValue(arg1);
    }
    if(splicedImagesList->count()>0)
    {
    splitDrawLines();
    splice_images();
    }
}

void MainWindow::splitDrawLines()
{
    QPixmap pix(splitOriginalImage);//copy of original image to draw on
    QPainter painter(&pix);
    QPen Red(QColor(0xFF, 0, 0, 0x80),1);
    painter.setPen(Red);

    //Draw outer border lines first. Needs to be offset a bit
    painter.drawLine(0,0, splitOriginalImage.width(), 0);
    painter.drawLine(0,splitOriginalImage.height()-1, splitOriginalImage.width(), splitOriginalImage.height()-1);
    painter.drawLine(0,0,0, splitOriginalImage.height());
    painter.drawLine(splitOriginalImage.width()-1,0, splitOriginalImage.width()-1, splitOriginalImage.height());

    int cellWidth = splitOriginalImage.width()/ui->splitColumnSpinBox->value();
    for(int i = 1; i<ui->splitColumnSpinBox->value(); i++)
    {
        painter.drawLine(cellWidth * i,0, cellWidth * i,splitOriginalImage.height());
    }

    int cellHeight = splitOriginalImage.height()/ui->splitRowSpinBox->value();
    for(int j = 1; j<ui->splitRowSpinBox->value(); j++)
    {
        painter.drawLine(0,cellHeight * j,splitOriginalImage.width(), cellHeight * j);
    }
    QIcon splitImageIcon(pix);
    splitImageButton->setIconSize(pix.rect().size());
    splitImageButton->setIcon(splitImageIcon);
    splitImageButton->setStyleSheet("QPushButton { background-color: transparent; border: 0px }");
    painter.end();

}

//Show preview of cell user clicks on
void MainWindow::splitPreviewImageClick()
{
    int x = splitImageButton->mapFromGlobal(QCursor::pos()).x();//Mouse click position
    int y = splitImageButton->mapFromGlobal(QCursor::pos()).y();

    int cellWidth = splitOriginalImage.width()/ui->splitColumnSpinBox->value();
    int cellHeight = splitOriginalImage.height()/ui->splitRowSpinBox->value();

    for (int i = 0; i<splicedImagesList->count(); i++)
    {
        if(x>=splicedImagesList->at(i).getStartingXPos() && x<=splicedImagesList->at(i).getStartingXPos()+cellWidth)
        {
            if(y>=splicedImagesList->at(i).getStartingYPos() && y<=splicedImagesList->at(i).getStartingYPos()+cellHeight)
            {
                int w = ui->splitImagePreviewLabel->width ();
                int h = ui->splitImagePreviewLabel->height ();
                QPixmap splicedImage(splicedImagesList->at(i).getImage());
                ui->splitImagePreviewLabel->setPixmap (splicedImage.scaled (w,h,Qt::KeepAspectRatio));
            }
        }
    }

}

void MainWindow::on_splitSaveButton_clicked()
{
    QFileDialog saveFilesDialog;
    QString splitBaseFolderName = saveFilesDialog.getSaveFileName();

    QDir().mkdir(splitBaseFolderName);//create folder

    int fileNumber = 1;
    for (int i = 0; i<splicedImagesList->count(); i++)
    {
        splicedImagesList->at(i).getImage().save(splitBaseFolderName + "/" + QString::number(fileNumber) + ".png");
        fileNumber++;
    }
    saveFilesDialog.close();

}

void MainWindow::splice_images()
{
    splicedImagesList->clear();
    //Splicing images and store useful information
    int cellWidth = splitOriginalImage.width()/ui->splitColumnSpinBox->value();
    int cellHeight = splitOriginalImage.height()/ui->splitRowSpinBox->value();
    int i = 0;//row
    while(i<=splitOriginalImage.height()-cellHeight){
        int j = 0; //col
        while(j<=splitOriginalImage.width()-cellWidth)
        {
            QPixmap copy = splitOriginalImage.copy( j, i, cellWidth, cellHeight);
            ImageFile splicedImage;
            splicedImage.setImage(copy);
            splicedImage.setWidth(cellWidth);
            splicedImage.setHeight(cellHeight);
            splicedImage.setStartingXPos(j);
            splicedImage.setStartingYPos(i);
            splicedImagesList->append(splicedImage);
            j = j+cellWidth;
        }
        i = i+cellHeight;
    }

    if(splitIsAnimating)
    {
        splitChangeNextAnimationImage();
    }
}

//Start or stop animation
void MainWindow::on_splitAnimateButton_clicked()
{
    if(splitIsAnimating)
    {
        splitIsAnimating = false;
        splitAnimationtimer->stop();
        ui->splitAnimateButton->setText("Animate");
        ui->splitAnimationPreviewLabel->clear();
    }
    else if (!splitIsAnimating && splicedImagesList->count()>0)
    {
        splitIsAnimating= true;
        splitAnimationtimer->start(ui->splitDelaySpinBox->value());
        ui->splitAnimateButton->setText("Stop");

    }
    //reset frames to first frame in group or beginning
    if(ui->splitPlayAllGroupsCheckBox->checkState()){
        splitAnimationIndex=0;
    }
    else
    {
        int startingFrame = (ui->splitColumnSpinBox->value() * ui->splitPlayGroupSpinBox->value()) - ui->splitColumnSpinBox->value();
        splitAnimationIndex = startingFrame;
    }
}


void MainWindow::splitChangeNextAnimationImage()//change animatin frame
{
    QPixmap previewImage(splicedImagesList->at(splitAnimationIndex).getImage());
    int w = ui->splitAnimationPreviewLabel->width ();
    int h = ui->splitAnimationPreviewLabel->height ();
    ui->splitAnimationPreviewLabel->setPixmap (previewImage.scaled (w,h,Qt::KeepAspectRatio));
}

//Change animation delay
void MainWindow::on_splitDelaySpinBox_valueChanged(int arg1)
{
    splitAnimationtimer->setInterval(arg1);
}

//Select which row to animate
void MainWindow::on_splitPlayGroupSpinBox_valueChanged(int arg1)
{
    int startingFrame = (ui->splitColumnSpinBox->value() * arg1) - ui->splitColumnSpinBox->value();
    splitAnimationIndex = startingFrame;
}


//toggle play all or only play one row
void MainWindow::on_splitPlayAllGroupsCheckBox_stateChanged(int arg1)
{
    if(arg1)
    {
        ui->splitPlayGroupSpinBox->setEnabled(false);
        splitAnimationIndex=0;
    }
    else
    {
        ui->splitPlayGroupSpinBox->setEnabled(true);
        int startingFrame = (ui->splitColumnSpinBox->value() * ui->splitPlayGroupSpinBox->value()) - ui->splitColumnSpinBox->value();
        splitAnimationIndex = startingFrame;
    }
}

void MainWindow::on_splitClearImageButton_clicked()
{
    splicedImagesList->clear();
    ui->splitTableWidget->clear();
    ui->splitImagePreviewLabel->clear();
    if(splitIsAnimating)
    {
        ui->splitAnimateButton->click();
    }
}
