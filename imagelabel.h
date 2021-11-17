#ifndef IMAGELABEL_H
#define IMAGELABEL_H

#include <QLabel>

class ImageLabel : public QLabel
{
    Q_OBJECT

public:
    ImageLabel();

    int gridCols() const;
    void setGridCols(int gridCols);

    int gridRows() const;
    void setGridRows(int gridRows);

    int gridLineWidth() const;
    void setGridLineWidth(int gridLineWidth);

    QColor gridColor() const;
    void setGridColor(const QColor &gridColor);

    bool gridVisible() const;
    void setGridVisible(bool gridVisible);

signals:
    void onMousePressEvent(QMouseEvent *ev);
    void onMouseMoveEvent(QMouseEvent *ev);
    void onMouseReleaseEvent(QMouseEvent *ev);
    void onMouseDoubleClickEvent(QMouseEvent *ev);
    void onWheelEvent(QWheelEvent *ev);

protected:
    void mousePressEvent(QMouseEvent *ev) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *ev) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *ev) Q_DECL_OVERRIDE;
    void mouseDoubleClickEvent(QMouseEvent *ev) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent *ev) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *ev) Q_DECL_OVERRIDE;

private:
    int mGridCols;
    int mGridRows;
    int mGridLineWidth;
    QColor mGridColor;
    bool mGridVisible;
};

#endif // IMAGELABEL_H
