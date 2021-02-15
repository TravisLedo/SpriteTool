#include "imagefile.h"
#include <string>
#include <cstring>

QString ImageFile::getPath() const
{
    return path;
}

void ImageFile::setPath(const QString &value)
{
    path = value;
}

QString ImageFile::getName() const
{
    return name;
}

void ImageFile::setName(const QString &value)
{
    name = value;
}

QPixmap ImageFile::getImage() const
{
    return image;
}

void ImageFile::setImage(const QPixmap &value)
{
    image = value;
}

int ImageFile::getWidth() const
{
    return width;
}

void ImageFile::setWidth(int value)
{
    width = value;
}

int ImageFile::getHeight() const
{
    return height;
}

void ImageFile::setHeight(int value)
{
    height = value;
}

int ImageFile::getStartingXPos() const
{
    return startingXPos;
}

void ImageFile::setStartingXPos(int value)
{
    startingXPos = value;
}

int ImageFile::getStartingYPos() const
{
    return startingYPos;
}

void ImageFile::setStartingYPos(int value)
{
    startingYPos = value;
}

ImageFile::ImageFile()
{

}



void myFunction() {
    // code to be executed
}



