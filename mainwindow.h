#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#ifndef QT_NO_PRINTER
#include <QPrinter>
#endif

QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;
class QUndoStack;
QT_END_NAMESPACE

#include "imagelabel.h"

namespace Ui {
class MainWindow;
}

class Grid : public QObject
{
    Q_OBJECT

public:
    QPoint offset() const { return m_offset; }
    void setOffset(QPoint offset)
    {
        m_offset = offset;
        emit offsetUpdated(m_offset);
    }

signals:
    void offsetUpdated(QPoint offset);

private:
    QPoint m_offset;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    bool loadFile(const QString &);

private slots:
    void open();
    void saveAs();
    void print();
    void copy();
    void paste();
    void zoomIn();
    void zoomOut();
    void normalSize();
    void fitToWindow();
    void about();

    void on_doubleSpinBoxGridSpacing_valueChanged(double value);

    void on_checkBoxShowGrid_toggled(bool checked);

    void on_pushButtonGridColor_clicked();

    void on_spinBoxRows_valueChanged(int value);

    void on_spinBoxColumns_valueChanged(int value);

private:
    void createActions();
    void createMenus();
    void updateActions();
    bool saveFile(const QString &fileName);
    void setImage(const QImage &newImage);
    void scaleImage(double factor);
    void adjustScrollBar(QScrollBar *scrollBar, double factor);
    void drawGrid();
    void updateGrid();

    Ui::MainWindow *ui;
    QImage imageWithoutGrid;
    QImage image;
    ImageLabel *imageLabel;
    QScrollArea *scrollArea;
    double scaleFactor;
    QColor gridColor;
    QPoint gridClickedPos;
    QPoint gridOffset;
    QPoint offsetOriginal;
    QUndoStack *undoStack;
    Grid m_grid;

#ifndef QT_NO_PRINTER
    QPrinter printer;
#endif

private slots:
    void onLabelMousePress(QMouseEvent *ev);
    void onLabelMouseMove(QMouseEvent *ev);
    void onLabelMouseRelease(QMouseEvent *ev);
    void onLabelMouseDoubleClick(QMouseEvent *ev);
    void on_spinBoxLineWidth_valueChanged(int value);
    void offsetUpdated(QPoint offset);
};

#endif // MAINWINDOW_H
