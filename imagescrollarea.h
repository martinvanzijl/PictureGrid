#ifndef IMAGESCROLLAREA_H
#define IMAGESCROLLAREA_H

#include <QScrollArea>

class ImageScrollArea : public QScrollArea
{
    Q_OBJECT

public:
    ImageScrollArea();

signals:
    void onCtrlWheelEvent(QWheelEvent *ev);

protected:
    void wheelEvent(QWheelEvent *ev) Q_DECL_OVERRIDE;
};

#endif // IMAGESCROLLAREA_H
