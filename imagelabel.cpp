#include "imagelabel.h"

ImageLabel::ImageLabel(QWidget *parent) : QLabel(parent)
{
    set = false;
}

QImage ImageLabel::sendImage()
{
    return this->grab().toImage();
}

void ImageLabel::loadedImage(bool send)
{
    set = send;
}

bool ImageLabel::haveSet()
{
    return set;
}
