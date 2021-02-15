#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <string.h>
#include <QLabel>
#include <QListWidgetItem>
#include <QSpinBox>
#include <imagefile.h>
#include <QVector>
#include <QPushButton>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setup();
    void setupMergeContent();
    void setupSplitContent();

    void mergeChangeNextAnimationImage();
    bool mergeCheckIfAllImagesSameSize(QVector<ImageFile*> list);

    void splitChangeNextAnimationImage();


private slots:
    void resizeEvent(QResizeEvent* event);

    //Merge Tab Content
    void mergeTimerUpdate();
    void mergeUpdateSheetTable();
    void on_mergeAddImagesButton_clicked();
    void on_mergeRemoveImageButton_clicked();
    void on_mergeClearImagesButton_clicked();
    void on_mergeSaveButton_clicked();
    void on_mergeDelaySpinBox_valueChanged(int arg1);
    void on_mergeColumnSpinBox_valueChanged(int arg1);
    void on_mergeAnimateButton_clicked();
    void mergePreviewImageClick();
    void on_mergeFileListWidget_currentRowChanged(int currentRow);
    void mergeRecalculateImagePositions();

    //Split Tab Content
    void splitTimerUpdate();
    void splitUpdateSheetTable();
    void on_splitAddImageButtton_clicked();
    void splitDrawLines();
    void on_splitSaveButton_clicked();
    void splice_images();
    void on_splitColumnSpinBox_valueChanged(int arg1);
    void on_splitAnimateButton_clicked();
    void on_splitDelaySpinBox_valueChanged(int arg1);
    void on_splitRowSpinBox_valueChanged(int arg1);
    void splitPreviewImageClick();
    void on_splitPlayAllGroupsCheckBox_stateChanged(int arg1);
    void on_splitPlayGroupSpinBox_valueChanged(int arg1);
    void on_splitClearImageButton_clicked();
    void on_mergePlayGroupSpinBox_valueChanged(int arg1);
    void on_mergePlayAllGroupsCheckBox_stateChanged(int arg1);

private:
    Ui::MainWindow *ui;

    //Merge Tab Content
    QTimer *mergeAnimationtimer;
    int mergeAnimationIndex;
    bool mergeIsAnimating;
    QPixmap mergeFinalImage;
    QPushButton *mergeImageButton;

    //Split Tab Content
    QPixmap splitOriginalImage;
    QVector<ImageFile> *splicedImagesList = new QVector<ImageFile>();//holds list of images
    QPushButton *splitImageButton;
    QTimer *splitAnimationtimer;
    int splitAnimationIndex;
    bool splitIsAnimating;


};
#endif // MAINWINDOW_H
