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



void MainWindow::setupMergeContent()
{
    //timer and bool for animation
    mergeAnimationtimer = new QTimer(this);
    mergeIsAnimating = false;

    connect(mergeAnimationtimer, SIGNAL(timeout()), this, SLOT(mergeTimerUpdate()));
    connect(ui->mergeFileListWidget->model(), SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)), this, SLOT(mergeUpdateSheetTable()));
}

//Gets called every x miliseconds. used to animate sprite
void MainWindow::mergeTimerUpdate()
{
    int startingFrame;
    int endingFrame;
    if(ui->mergePlayAllGroupsCheckBox->checkState())
    {
        startingFrame = 0;
        endingFrame = ui->mergeFileListWidget->count()-1;
    }
    else{
        startingFrame = (ui->mergeColumnSpinBox->value() * ui->mergePlayGroupSpinBox->value()) - ui->mergeColumnSpinBox->value();
        endingFrame = (ui->mergeColumnSpinBox->value() * ui->mergePlayGroupSpinBox->value())-1;
        //Check if last row has empty images.
        if(endingFrame>ui->mergeFileListWidget->count()-1)
        {
            endingFrame = ui->mergeFileListWidget->count()-1;
        }
    }

    if(mergeAnimationIndex<endingFrame)
    {
        mergeAnimationIndex++;
        mergeChangeNextAnimationImage();
    }
    else
    {
        mergeAnimationIndex = startingFrame;
        mergeChangeNextAnimationImage();
    }

}


void MainWindow::mergeChangeNextAnimationImage()//change animatin frame
{
    ImageFile * imageItem = static_cast<ImageFile*>(ui->mergeFileListWidget->item(mergeAnimationIndex));
    int w = ui->mergeAnimationPreviewLabel->width ();
    int h = ui->mergeAnimationPreviewLabel->height ();
    ui->mergeAnimationPreviewLabel->setPixmap (imageItem->getImage().scaled (w,h,Qt::KeepAspectRatio));
}


void MainWindow::mergeUpdateSheetTable()
{
    mergeRecalculateImagePositions();

    ui->mergeTableWidget->setRowCount(1);
    ui->mergeTableWidget->setColumnCount(1);

    if(ui->mergeFileListWidget->count()>0)
    {
        int cellWidth = static_cast<ImageFile*>(ui->mergeFileListWidget->item(0))->getWidth();//get dementions of first image, they should all be the same
        int cellHeight = static_cast<ImageFile*>(ui->mergeFileListWidget->item(0))->getHeight();
        int numberOfRows = ui->mergeFileListWidget->count()/ui->mergeColumnSpinBox->value() + (ui->mergeFileListWidget->count() % ui->mergeColumnSpinBox->value() != 0); //calculate how many rows needed

        mergeFinalImage = QPixmap(cellWidth*ui->mergeColumnSpinBox->value(), cellHeight*numberOfRows);
        QPixmap pix(cellWidth*ui->mergeColumnSpinBox->value(), cellHeight*numberOfRows);
                pix.fill( Qt::transparent );
        QPainter painter(&pix);

        for (int i = 0;i<numberOfRows; i++)//generate rows
        {
            ui->mergeTableWidget->insertRow(ui->mergeTableWidget->rowCount());
        }

        int currentColumn = 0;
        int currentRow = 0;
        for (int i = 0;i< ui->mergeFileListWidget->count();i++)
        {
            ImageFile *currentImageFile = static_cast<ImageFile*>(ui->mergeFileListWidget->item(i));//reference current image
            painter.drawPixmap(currentImageFile->getStartingXPos(),
                               currentImageFile->getStartingYPos(),
                               currentImageFile->getWidth(),
                               currentImageFile->getHeight(),
                               currentImageFile->getImage());//Draw images onto big canvas image

            if(currentColumn == ui->mergeColumnSpinBox->value()-1)
            {
                currentRow++;
                currentColumn = 0;
            }
            else
            {
                currentColumn++;
            }
        }
        painter.end();

        mergeFinalImage = pix;
        QIcon mergeImageIcon(mergeFinalImage);
        mergeImageButton = new QPushButton();
        connect(mergeImageButton, SIGNAL(clicked()),this, SLOT(mergePreviewImageClick()));
        mergeImageButton->setIconSize(mergeFinalImage.size());
        mergeImageButton->setIcon(mergeImageIcon);
        mergeImageButton->setStyleSheet("background-color: transparent; border: 0px ");
        mergeImageButton->setFixedSize(mergeFinalImage.size());

        ui->mergeTableWidget->setCellWidget(0, 0, mergeImageButton);//put button into cell
        ui->mergeTableWidget->horizontalHeader()->setDefaultSectionSize(mergeImageButton->width());
        ui->mergeTableWidget->verticalHeader()->setDefaultSectionSize(mergeImageButton->height());

    }

}

//When item is highlighted, show a preview
void MainWindow::on_mergeFileListWidget_currentRowChanged(int currentRow)
{
    QListWidgetItem * current = ui->mergeFileListWidget->item(currentRow);
    if(current != NULL){
        ImageFile * imageItem = static_cast<ImageFile*>(current);
        int w = ui->mergeImagePreviewLabel->width ();
        int h = ui->mergeImagePreviewLabel->height ();
        ui->mergeImagePreviewLabel->setPixmap (imageItem->getImage().scaled (w,h,Qt::KeepAspectRatio));
    }
    else
    {
        ui->mergeImagePreviewLabel->clear();
    }
}


void MainWindow::on_mergeDelaySpinBox_valueChanged(int arg1)
{
    mergeAnimationtimer->setInterval(arg1);
}


void MainWindow::on_mergeSaveButton_clicked()
{
    if(ui->mergeFileListWidget->count()>0){
        //No images to create a sheet
        QFileDialog saveFilesDialog;
        QString imagePath = saveFilesDialog.getSaveFileName(this,tr("Save File"),"",tr("PNG (*.png)" ));
        if(imagePath.length()>0){
        mergeFinalImage.save(imagePath);
        }
        saveFilesDialog.close();
    }
}


void MainWindow::on_mergeAddImagesButton_clicked()
{
    QFileDialog openFilesDialog;
    QStringList fileNames = openFilesDialog.getOpenFileNames(this, tr("Open File"),"/path/to/file/",tr("PNG Files (*.png)")); //store list of path/filenames
    QVector<ImageFile*> tempList;

    if(fileNames.count()>0)
    {
        for(int i = 0; i< fileNames.count(); i++)
        {
            ImageFile *imageItem = new ImageFile();
            QStringList elements = fileNames[i].split('/');
            imageItem->setPath(fileNames[i]);
            imageItem->setName(elements[elements.count()-1]);
            imageItem->setImage(imageItem->getPath());
            imageItem->setWidth(imageItem->getImage().size().width());
            imageItem->setHeight(imageItem->getImage().size().height());

            QString itemText; //what to display on the list
            itemText.append(imageItem->getName());
            //set text and icon on list
            imageItem->QListWidgetItem::setText(itemText);
            imageItem->QListWidgetItem::setIcon(QIcon(fileNames[i]));

            tempList.append(imageItem);
        }
        if(mergeCheckIfAllImagesSameSize(tempList))//only add images to the listwidget if they all are the same size
        {
            //add item to list
            for(int i = 0; i< tempList.count(); i++)
            {
                ImageFile * tempImage = static_cast<ImageFile*>(tempList.at(i));
                ui->mergeFileListWidget->addItem(tempImage);
            }
        }
        else//conflict with image file sizes
        {
            QMessageBox messageBox;
            messageBox.critical(0,"Error","Dimension conflicts!");
            messageBox.setFixedSize(500,200);
        }
        ui->mergeFileListWidget->setCurrentItem(NULL);
        mergeRecalculateImagePositions();
        mergeUpdateSheetTable();
    }
    openFilesDialog.close();

}


bool MainWindow::mergeCheckIfAllImagesSameSize(QVector<ImageFile*> list)
{

    for(int i = 1; i< list.count(); i++)//check if all selected images are the same size
    {
        ImageFile * previousItem = static_cast<ImageFile*>(list.at(i-1));
        ImageFile * currentItem = static_cast<ImageFile*>(list.at(i));
        if(currentItem->getWidth() != previousItem->getWidth() || currentItem->getHeight() != previousItem->getHeight())
        {
            return false;
        }
    }

    ImageFile * firstItemFromListWidget = static_cast<ImageFile*>(ui->mergeFileListWidget->item(0));
    if(ui->mergeFileListWidget->count()>0 && list.at(0)->getWidth() != firstItemFromListWidget->getWidth())//check if one image from the list and one from listwidgetview are the same size.
    {
        return false;
    }
    return true;
}



void MainWindow::on_mergeColumnSpinBox_valueChanged(int arg1)
{
    int numberOfRows = ui->mergeFileListWidget->count()/arg1 + (ui->mergeFileListWidget->count() % arg1 != 0); //calculate how many rows needed
    ui->mergePlayGroupSpinBox->setMaximum(numberOfRows);

    mergeUpdateSheetTable();
}


void MainWindow::mergeRecalculateImagePositions()
{
    int startingX = 0;
    int startingY = 0;
    for(int i = 0; i< ui->mergeFileListWidget->count(); i++)
    {
        ImageFile * imageItem = static_cast<ImageFile*>(ui->mergeFileListWidget->item(i));

        imageItem->setStartingXPos(startingX);
        imageItem->setStartingYPos(startingY);

         startingX = startingX + imageItem->getWidth();

        if(startingX>=imageItem->getWidth()*ui->mergeColumnSpinBox->value())
        {
            startingX = 0;
            startingY = startingY + imageItem->getHeight();
        }
    }
}



void MainWindow::on_mergeRemoveImageButton_clicked()
{
    ui->mergeFileListWidget->takeItem(ui->mergeFileListWidget->currentRow());

    if(ui->mergeFileListWidget->count()<=0)
    {
        ui->mergeFileListWidget->setCurrentItem(NULL);
    }

    mergeUpdateSheetTable();
}

void MainWindow::on_mergeClearImagesButton_clicked()
{
    if(mergeIsAnimating)
    {
        on_mergeAnimateButton_clicked();
    }

    ui->mergeFileListWidget->clear();
    mergeUpdateSheetTable();
}



void MainWindow::on_mergeAnimateButton_clicked()
{
    if(mergeIsAnimating)
    {
        mergeIsAnimating = false;
        mergeAnimationtimer->stop();
        ui->mergeAnimateButton->setText("Animate");
        ui->mergeAnimationPreviewLabel->clear();
    }
    else if (!mergeIsAnimating && ui->mergeFileListWidget->count()>0)
    {
        mergeIsAnimating= true;
        mergeAnimationtimer->start(ui->mergeDelaySpinBox->value());
        ui->mergeAnimateButton->setText("Stop");
    }

    //reset frames to first frame in group or beginning
    if(ui->mergePlayAllGroupsCheckBox->checkState())
    {
        mergeAnimationIndex=0;
    }
    else{
        int startingFrame = (ui->mergeColumnSpinBox->value() * ui->mergePlayGroupSpinBox->value()) - ui->mergeColumnSpinBox->value();
        mergeAnimationIndex = startingFrame;
    }
}


void MainWindow::on_mergePlayGroupSpinBox_valueChanged(int arg1)
{
    int startingFrame = (ui->mergeColumnSpinBox->value() * arg1) - ui->mergeColumnSpinBox->value();
    mergeAnimationIndex = startingFrame;
}

void MainWindow::on_mergePlayAllGroupsCheckBox_stateChanged(int arg1)
{
    if(arg1){
        ui->mergePlayGroupSpinBox->setEnabled(false);
        mergeAnimationIndex=0;
    }
    else{
        ui->mergePlayGroupSpinBox->setEnabled(true);
        int startingFrame = (ui->mergeColumnSpinBox->value() * ui->mergePlayGroupSpinBox->value()) - ui->mergeColumnSpinBox->value();
        mergeAnimationIndex = startingFrame;
    }
}


void MainWindow::mergePreviewImageClick()
{
    int x = mergeImageButton->mapFromGlobal(QCursor::pos()).x();//Mouse click position
    int y = mergeImageButton->mapFromGlobal(QCursor::pos()).y();

    for (int i = 0; i<ui->mergeFileListWidget->count(); i++) {
        ImageFile * currentImage = static_cast<ImageFile*>(ui->mergeFileListWidget->item(i));
        if(x>=currentImage->getStartingXPos() && x<=currentImage->getStartingXPos()+currentImage->getWidth())
        {
            if(y>=currentImage->getStartingYPos() && y<=currentImage->getStartingYPos()+currentImage->getHeight())
            {
                    ui->mergeFileListWidget->setCurrentRow(i);
            }
        }
    }

}
