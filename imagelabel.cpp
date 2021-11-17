#include "imagelabel.h"

#include <QPainter>
#include <QPen>

ImageLabel::ImageLabel() :
    mGridCols(3),
    mGridRows(3),
    mGridLineWidth(1),
    mGridColor(Qt::black)
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

void ImageLabel::wheelEvent(QWheelEvent *ev)
{
    emit onWheelEvent(ev);
}

void ImageLabel::paintEvent(QPaintEvent *ev)
{
    // Draw normally.
    QLabel::paintEvent(ev);

    // Exit if grid is not visible.
    if (!mGridVisible)
    {
        return;
    }

    // Draw grid.
    QPen pen(mGridColor);
    pen.setWidth(mGridLineWidth);

    QPainter painter(this);
    painter.setPen(pen);

    QPoint gridOffset;

    for(int col = 0; col <= mGridCols; ++col)
    {
        double fraction = static_cast<double> (col) / static_cast<double> (mGridCols);
        int x = gridOffset.x() + (width() * fraction);
        painter.drawLine(x, 0, x, height());
    }

    for(int row = 0; row <= mGridRows; ++row)
    {
        double fraction = static_cast<double> (row) / static_cast<double> (mGridRows);
        int y = gridOffset.y() + (height() * fraction);
        painter.drawLine(0, y, width(), y);
    }
}

bool ImageLabel::gridVisible() const
{
    return mGridVisible;
}

void ImageLabel::setGridVisible(bool gridVisible)
{
    mGridVisible = gridVisible;
}

QColor ImageLabel::gridColor() const
{
    return mGridColor;
}

void ImageLabel::setGridColor(const QColor &gridColor)
{
    mGridColor = gridColor;
}

int ImageLabel::gridLineWidth() const
{
    return mGridLineWidth;
}

void ImageLabel::setGridLineWidth(int gridLineWidth)
{
    mGridLineWidth = gridLineWidth;
}

int ImageLabel::gridRows() const
{
    return mGridRows;
}

void ImageLabel::setGridRows(int gridRows)
{
    mGridRows = gridRows;
}

int ImageLabel::gridCols() const
{
    return mGridCols;
}

void ImageLabel::setGridCols(int gridCols)
{
    mGridCols = gridCols;
}
