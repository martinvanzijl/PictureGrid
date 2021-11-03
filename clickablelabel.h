#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>

class ClickableLabel : public QLabel
{
    Q_OBJECT

public:
    ClickableLabel(QWidget *parent = nullptr);

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *ev) Q_DECL_OVERRIDE;
};

#endif // CLICKABLELABEL_H
