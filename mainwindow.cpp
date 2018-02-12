#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtWidgets>
#if defined(QT_PRINTSUPPORT_LIB)
#include <QtPrintSupport/qtprintsupportglobal.h>
//#if QT_CONFIG(printdialog)
#include <QPrintDialog>
//#endif
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    imageLabel(new QLabel),
    scrollArea(new QScrollArea),
    scaleFactor(1),
    gridColor(Qt::black)
{
    ui->setupUi(this);

    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);

    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(imageLabel);
    scrollArea->setVisible(false);

    ui->horizontalLayout->addWidget(scrollArea);

    //setCentralWidget(scrollArea);

    createActions();

    resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::loadFile(const QString &fileName)
{
    QImageReader reader(fileName);
    reader.setAutoTransform(true);
    const QImage newImage = reader.read();
    if (newImage.isNull()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1: %2")
                                 .arg(QDir::toNativeSeparators(fileName), reader.errorString()));
        return false;
    }

    setImage(newImage);

    setWindowFilePath(fileName);

    const QString message = tr("Opened \"%1\", %2x%3, Depth: %4")
        .arg(QDir::toNativeSeparators(fileName)).arg(image.width()).arg(image.height()).arg(image.depth());
    statusBar()->showMessage(message);
    return true;
}

void MainWindow::setImage(const QImage &newImage)
{
    image = newImage;
    imageWithoutGrid = newImage;
    drawGrid();
    imageLabel->setPixmap(QPixmap::fromImage(image));
    scaleFactor = 1.0;

    scrollArea->setVisible(true);
    ui->printAct->setEnabled(true);
    ui->fitToWindowAct->setEnabled(true);
    updateActions();

    if (!ui->fitToWindowAct->isChecked())
        imageLabel->adjustSize();
}

bool MainWindow::saveFile(const QString &fileName)
{
    QImageWriter writer(fileName);

    if (!writer.write(image)) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot write %1: %2")
                                 .arg(QDir::toNativeSeparators(fileName)), writer.errorString());
        return false;
    }
    const QString message = tr("Wrote \"%1\"").arg(QDir::toNativeSeparators(fileName));
    statusBar()->showMessage(message);
    return true;
}

static void initializeImageFileDialog(QFileDialog &dialog, QFileDialog::AcceptMode acceptMode)
{
    static bool firstDialog = true;

    if (firstDialog) {
        firstDialog = false;
        const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
        dialog.setDirectory(picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
    }

    if (acceptMode == QFileDialog::AcceptSave)
        dialog.setDefaultSuffix("jpg");
}

void MainWindow::open()
{
    QFileDialog dialog(this, tr("Open File"));
    initializeImageFileDialog(dialog, QFileDialog::AcceptOpen);

    while (dialog.exec() == QDialog::Accepted && !loadFile(dialog.selectedFiles().first())) {}
}

void MainWindow::saveAs()
{
    QFileDialog dialog(this, tr("Save File As"));
    initializeImageFileDialog(dialog, QFileDialog::AcceptSave);

    while (dialog.exec() == QDialog::Accepted && !saveFile(dialog.selectedFiles().first())) {}
}

void MainWindow::print()
{
    Q_ASSERT(imageLabel->pixmap());
//#if QT_CONFIG(printdialog)
    QPrintDialog dialog(&printer, this);
    if (dialog.exec()) {
        QPainter painter(&printer);
        QRect rect = painter.viewport();
        QSize size = imageLabel->pixmap()->size();
        size.scale(rect.size(), Qt::KeepAspectRatio);
        painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
        painter.setWindow(imageLabel->pixmap()->rect());
        painter.drawPixmap(0, 0, *imageLabel->pixmap());
    }
//#endif
}

void MainWindow::createMenus()
{

}

void MainWindow::copy()
{
#ifndef QT_NO_CLIPBOARD
    QGuiApplication::clipboard()->setImage(image);
#endif // !QT_NO_CLIPBOARD
}

#ifndef QT_NO_CLIPBOARD
static QImage clipboardImage()
{
    if (const QMimeData *mimeData = QGuiApplication::clipboard()->mimeData()) {
        if (mimeData->hasImage()) {
            const QImage image = qvariant_cast<QImage>(mimeData->imageData());
            if (!image.isNull())
                return image;
        }
    }
    return QImage();
}
#endif // !QT_NO_CLIPBOARD

void MainWindow::paste()
{
#ifndef QT_NO_CLIPBOARD
    const QImage newImage = clipboardImage();
    if (newImage.isNull()) {
        statusBar()->showMessage(tr("No image in clipboard"));
    } else {
        setImage(newImage);
        setWindowFilePath(QString());
        const QString message = tr("Obtained image from clipboard, %1x%2, Depth: %3")
            .arg(newImage.width()).arg(newImage.height()).arg(newImage.depth());
        statusBar()->showMessage(message);
    }
#endif // !QT_NO_CLIPBOARD
}

void MainWindow::zoomIn()
{
    scaleImage(1.25);
}

void MainWindow::zoomOut()
{
    scaleImage(0.8);
}

void MainWindow::normalSize()
{
    imageLabel->adjustSize();
    scaleFactor = 1.0;
}

void MainWindow::fitToWindow()
{
    bool fitToWindow = ui->fitToWindowAct->isChecked();
    scrollArea->setWidgetResizable(fitToWindow);
    if (!fitToWindow)
        normalSize();
    updateActions();
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Image Viewer"),
            tr("<p>The <b>Image Viewer</b> example shows how to combine QLabel "
               "and QScrollArea to display an image. QLabel is typically used "
               "for displaying a text, but it can also display an image. "
               "QScrollArea provides a scrolling view around another widget. "
               "If the child widget exceeds the size of the frame, QScrollArea "
               "automatically provides scroll bars. </p><p>The example "
               "demonstrates how QLabel's ability to scale its contents "
               "(QLabel::scaledContents), and QScrollArea's ability to "
               "automatically resize its contents "
               "(QScrollArea::widgetResizable), can be used to implement "
               "zooming and scaling features. </p><p>In addition the example "
               "shows how to use QPainter to print an image.</p>"));
}

void MainWindow::createActions()
{
    connect(ui->openAct, &QAction::triggered, this, &MainWindow::open);
    ui->openAct->setShortcut(QKeySequence::Open);

    connect(ui->saveAsAct, &QAction::triggered, this, &MainWindow::saveAs);
    ui->saveAsAct->setEnabled(false);

    connect(ui->printAct, &QAction::triggered, this, &MainWindow::print);
    ui->printAct->setShortcut(QKeySequence::Print);
    ui->printAct->setEnabled(false);

    connect(ui->exitAct, &QAction::triggered, this, &QWidget::close);

    connect(ui->copyAct, &QAction::triggered, this, &MainWindow::copy);
    ui->copyAct->setShortcut(QKeySequence::Copy);
    ui->copyAct->setEnabled(false);

    connect(ui->pasteAct, &QAction::triggered, this, &MainWindow::paste);

    connect(ui->zoomInAct, &QAction::triggered, this, &MainWindow::zoomIn);
    ui->zoomInAct->setShortcut(QKeySequence::ZoomIn);
    ui->zoomInAct->setEnabled(false);

    connect(ui->zoomOutAct, &QAction::triggered, this, &MainWindow::zoomOut);
    ui->zoomOutAct->setShortcut(QKeySequence::ZoomOut);
    ui->zoomOutAct->setEnabled(false);

    connect(ui->normalSizeAct, &QAction::triggered, this, &MainWindow::normalSize);
    ui->normalSizeAct->setEnabled(false);

    connect(ui->fitToWindowAct, &QAction::triggered, this, &MainWindow::fitToWindow);
    ui->fitToWindowAct->setEnabled(false);
    ui->fitToWindowAct->setCheckable(true);

    connect(ui->aboutAct, &QAction::triggered, this, &MainWindow::about);
    connect(ui->aboutQtAct, &QAction::triggered, this, &QApplication::aboutQt);
}

void MainWindow::updateActions()
{
    ui->saveAsAct->setEnabled(!image.isNull());
    ui->copyAct->setEnabled(!image.isNull());
    ui->zoomInAct->setEnabled(!ui->fitToWindowAct->isChecked());
    ui->zoomOutAct->setEnabled(!ui->fitToWindowAct->isChecked());
    ui->normalSizeAct->setEnabled(!ui->fitToWindowAct->isChecked());
}

void MainWindow::scaleImage(double factor)
{
    Q_ASSERT(imageLabel->pixmap());
    scaleFactor *= factor;
    imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());

    adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(scrollArea->verticalScrollBar(), factor);

    ui->zoomInAct->setEnabled(scaleFactor < 3.0);
    ui->zoomOutAct->setEnabled(scaleFactor > 0.333);
}

void MainWindow::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value()
                            + ((factor - 1) * scrollBar->pageStep()/2)));
}

void MainWindow::drawGrid()
{
    int gridSpacing = (int) ui->doubleSpinBoxGridSpacing->value();

    QPainter painter(&image);
    painter.setPen(gridColor);

    for(int x = 0; x < image.width(); x += gridSpacing)
    {
        painter.drawLine(x, 0, x, image.height());
    }

    for(int y = 0; y < image.height(); y += gridSpacing)
    {
        painter.drawLine(0, y, image.width(), y);
    }
}

void MainWindow::updateGrid()
{
    image = imageWithoutGrid;

    if(ui->checkBoxShowGrid->isChecked())
    {
        drawGrid();
    }

    imageLabel->setPixmap(QPixmap::fromImage(image));
}

void MainWindow::on_doubleSpinBoxGridSpacing_valueChanged(double value)
{
    Q_UNUSED(value)

    updateGrid();
}

void MainWindow::on_checkBoxShowGrid_toggled(bool checked)
{
    Q_UNUSED(checked)

    updateGrid();
}

void MainWindow::on_pushButtonGridColor_clicked()
{
    QColor newColor = QColorDialog::getColor(gridColor);
    if (newColor.isValid())
    {
        gridColor = newColor;
        updateGrid();
    }
}
