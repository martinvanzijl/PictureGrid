#include "imagescrollarea.h"

#include <QWheelEvent>

ImageScrollArea::ImageScrollArea()
{

}

void ImageScrollArea::wheelEvent(QWheelEvent *ev)
{
    // Use Ctrl+Mouse Wheel to zoom.
    if (ev->modifiers() == Qt::ControlModifier)
    {
        emit onCtrlWheelEvent(ev);
    }
    else
    {
        // Handle normally.
        QScrollArea::wheelEvent(ev);
    }
}
