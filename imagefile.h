#ifndef IMAGEFILE_H
#define IMAGEFILE_H
#include <string>
#include <cstring>
#include <QString>
#include <QListWidgetItem>
#include <QTableWidgetItem>
#include <QPixmap>

//A custom class that contains data for images and also capable of being a QListWidgetItem
class ImageFile: public QListWidgetItem
{
private:
    QString path;
    QString name;
    QPixmap image;
    int width;
    int height;
    int startingXPos;
    int startingYPos;
public:

    ImageFile();
    QString getPath() const;
    void setPath(const QString &value);
    QString getName() const;
    void setName(const QString &value);
    QPixmap getImage() const;
    void setImage(const QPixmap &value);
    int getWidth() const;
    void setWidth(int value);
    int getHeight() const;
    void setHeight(int value);
    int getStartingXPos() const;
    void setStartingXPos(int value);
    int getStartingYPos() const;
    void setStartingYPos(int value);
};

#endif // IMAGEFILE_H
