#include "imagelabel.h"

ImageLabel::ImageLabel()
{

}

void ImageLabel::mousePressEvent(QMouseEvent *ev)
{
    QLabel::mousePressEvent(ev);

    emit onMousePressEvent(ev);
}

void ImageLabel::mouseMoveEvent(QMouseEvent *ev)
{
    QLabel::mouseMoveEvent(ev);

    emit onMouseMoveEvent(ev);
}

void ImageLabel::mouseReleaseEvent(QMouseEvent *ev)
{
    QLabel::mouseReleaseEvent(ev);

    emit onMouseReleaseEvent(ev);
}

void ImageLabel::mouseDoubleClickEvent(QMouseEvent *ev)
{
    QLabel::mouseDoubleClickEvent(ev);

    emit onMouseDoubleClickEvent(ev);
}
