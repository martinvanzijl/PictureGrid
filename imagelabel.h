#ifndef IMAGELABEL_H
#define IMAGELABEL_H

#include <QLabel>

class ImageLabel : public QLabel
{
    Q_OBJECT

public:
    ImageLabel();

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
};

#endif // IMAGELABEL_H
