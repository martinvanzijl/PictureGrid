#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtWidgets>
#if defined(QT_PRINTSUPPORT_LIB)
#include <QtPrintSupport/qtprintsupportglobal.h>
//#if QT_CONFIG(printdialog)
#include <QPrintDialog>
//#endif
#endif
#include <QUndoStack>
#include <QDesktopWidget>

class MoveGridCommand: public QUndoCommand
{
public:
    MoveGridCommand(Grid * grid, QPoint offsetBefore, QPoint offsetAfter)
        : m_grid(grid),
          m_offsetBefore(offsetBefore),
          m_offsetAfter(offsetAfter)
    {
        setText("move grid");
    }
    virtual void undo()
        { m_grid->setOffset(m_offsetBefore); }
    virtual void redo()
        { m_grid->setOffset(m_offsetAfter); }
private:
    Grid * m_grid;
    QPoint m_offsetBefore;
    QPoint m_offsetAfter;
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    imageLabel(new ImageLabel),
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

    undoStack = new QUndoStack(this);
    ui->menuEdit->addSeparator();
    ui->menuEdit->addAction(undoStack->createUndoAction(this));
    ui->menuEdit->addAction(undoStack->createRedoAction(this));

    updateColorButtonIcon();

    // Create "Recent Files" menu.
    QMenu *recentMenu = new QMenu(tr("Recent..."), this);
    ui->menuFile->insertMenu(ui->exitAct, recentMenu);
    connect(recentMenu, &QMenu::aboutToShow, this, &MainWindow::updateRecentFileActions);
    recentFileSubMenuAct = recentMenu->menuAction();

    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActs[i] = recentMenu->addAction(QString(), this, &MainWindow::openRecentFile);
        recentFileActs[i]->setVisible(false);
    }

    recentFileSeparator = ui->menuFile->insertSeparator(ui->exitAct);

    setRecentFilesVisible(hasRecentFiles());
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

    prependToRecentFiles(fileName);

    // Update window title.
    QFileInfo info(fileName);
    setWindowTitle(info.fileName() + " - PictureGrid");

    return true;
}

void MainWindow::centerOnScreen()
{
    QRect screenRect = QApplication::desktop()->screenGeometry();
    QPoint offset(width() / 2, height() / 2);
    move(screenRect.center() - offset);
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

    prependToRecentFiles(fileName);

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
    QMessageBox::about(this, tr("About PictureGrid"),
            tr("<p>PictureGrid is an application that lets you add a grid to a picture.</p>"
               "<p>It is open-source and made using the Qt Framework.</p>"));
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

    connect(imageLabel, &ImageLabel::onMousePressEvent, this, &MainWindow::onLabelMousePress);
    connect(imageLabel, &ImageLabel::onMouseMoveEvent, this, &MainWindow::onLabelMouseMove);
    connect(imageLabel, &ImageLabel::onMouseReleaseEvent, this, &MainWindow::onLabelMouseRelease);
    connect(imageLabel, &ImageLabel::onMouseDoubleClickEvent, this, &MainWindow::onLabelMouseDoubleClick);
    connect(imageLabel, &ImageLabel::onWheelEvent, this, &MainWindow::onLabelWheelEvent);

    connect(&m_grid, &Grid::offsetUpdated, this, &MainWindow::offsetUpdated);
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
    int cols = ui->spinBoxColumns->value();
//    int horizontalSpacing = image.width() / cols;

    int rows = ui->spinBoxRows->value();
//    int verticalSpacing = image.height() / rows;

    int lineWidth = ui->spinBoxLineWidth->value();

    QPen pen(gridColor);
    pen.setWidth(lineWidth);

    QPainter painter(&image);
    painter.setPen(pen);

//    for(int x = gridOffset.x(); x < image.width(); x += horizontalSpacing)
//    {
//        painter.drawLine(x, 0, x, image.height());
//    }

    for(int col = 0; col < cols; ++col)
    {
        double fraction = static_cast<double> (col) / static_cast<double> (cols);
        int x = gridOffset.x() + (image.width() * fraction);
        painter.drawLine(x, 0, x, image.height());
    }

//    for(int y = gridOffset.y(); y < image.height(); y += verticalSpacing)
//    {
//        painter.drawLine(0, y, image.width(), y);
//    }

    for(int row = 0; row < rows; ++row)
    {
        double fraction = static_cast<double> (row) / static_cast<double> (rows);
        int y = gridOffset.y() + (image.height() * fraction);
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

void MainWindow::updateColorButtonIcon()
{
    QPixmap iconPixmap(16, 16);
    iconPixmap.fill(gridColor);
    QIcon icon(iconPixmap);
    ui->pushButtonGridColor->setIcon(icon);
    ui->pushButtonGridColor->setText("");
}

void MainWindow::onLabelMousePress(QMouseEvent *ev)
{
    gridClickedPos = ev->pos();
    offsetOriginal = gridOffset;
}

void MainWindow::onLabelMouseMove(QMouseEvent *ev)
{
    QPoint moveAmount = ev->pos() - gridClickedPos;
    gridOffset += moveAmount;
    gridClickedPos = ev->pos();

    updateGrid();
}

void MainWindow::onLabelMouseRelease(QMouseEvent *ev)
{
    Q_UNUSED(ev);

    auto command = new MoveGridCommand(&m_grid, offsetOriginal, gridOffset);
    undoStack->push(command);
}

void MainWindow::onLabelMouseDoubleClick(QMouseEvent *ev)
{
    Q_UNUSED(ev)

    // Reset offset.
    gridOffset.setX(0);
    gridOffset.setY(0);

    updateGrid();
}

void MainWindow::onLabelWheelEvent(QWheelEvent *ev)
{
    if (ev->modifiers() == Qt::ControlModifier)
    {
        QPoint numDegrees = ev->angleDelta() / 8;

        if (!numDegrees.isNull()) {
            if (numDegrees.y() > 0) {
                zoomIn();
            }
            else {
                zoomOut();
            }
        }

        ev->accept();
    }
}

void MainWindow::on_checkBoxShowGrid_toggled(bool checked)
{
    Q_UNUSED(checked)

    updateGrid();
}

void MainWindow::on_pushButtonGridColor_clicked()
{
    QColor newColor = QColorDialog::getColor(gridColor, this, "Select Grid Color");
    if (newColor.isValid())
    {
        gridColor = newColor;
        updateGrid();
        updateColorButtonIcon();
    }
}

void MainWindow::on_spinBoxRows_valueChanged(int value)
{
    Q_UNUSED(value)
    updateGrid();
}

void MainWindow::on_spinBoxColumns_valueChanged(int value)
{
    Q_UNUSED(value)
    updateGrid();
}

void MainWindow::on_spinBoxLineWidth_valueChanged(int value)
{
    Q_UNUSED(value)
    updateGrid();
}

void MainWindow::offsetUpdated(QPoint offset)
{
    gridOffset = offset;
    updateGrid();
}

static inline QString recentFilesKey() { return QStringLiteral("recentFileList"); }
static inline QString fileKey() { return QStringLiteral("file"); }

static QStringList readRecentFiles(QSettings &settings)
{
    QStringList result;
    const int count = settings.beginReadArray(recentFilesKey());
    for (int i = 0; i < count; ++i) {
        settings.setArrayIndex(i);
        result.append(settings.value(fileKey()).toString());
    }
    settings.endArray();
    return result;
}

static void writeRecentFiles(const QStringList &files, QSettings &settings)
{
    const int count = files.size();
    settings.beginWriteArray(recentFilesKey());
    for (int i = 0; i < count; ++i) {
        settings.setArrayIndex(i);
        settings.setValue(fileKey(), files.at(i));
    }
    settings.endArray();
}

bool MainWindow::hasRecentFiles()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const int count = settings.beginReadArray(recentFilesKey());
    settings.endArray();
    return count > 0;
}

void MainWindow::prependToRecentFiles(const QString &fileName)
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

    const QStringList oldRecentFiles = readRecentFiles(settings);
    QStringList recentFiles = oldRecentFiles;
    recentFiles.removeAll(fileName);
    recentFiles.prepend(fileName);
    if (oldRecentFiles != recentFiles)
        writeRecentFiles(recentFiles, settings);

    setRecentFilesVisible(!recentFiles.isEmpty());
}

void MainWindow::setRecentFilesVisible(bool visible)
{
    recentFileSubMenuAct->setVisible(visible);
    recentFileSeparator->setVisible(visible);
}

void MainWindow::updateRecentFileActions()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

    const QStringList recentFiles = readRecentFiles(settings);
    const int count = qMin(int(MaxRecentFiles), recentFiles.size());
    int i = 0;
    for ( ; i < count; ++i) {
        const QString fileName = QFileInfo(recentFiles.at(i)).fileName();
        recentFileActs[i]->setText(tr("&%1 %2").arg(i + 1).arg(fileName));
        recentFileActs[i]->setData(recentFiles.at(i));
        recentFileActs[i]->setVisible(true);
    }
    for ( ; i < MaxRecentFiles; ++i)
        recentFileActs[i]->setVisible(false);
}

void MainWindow::openRecentFile()
{
    if (const QAction *action = qobject_cast<const QAction *>(sender()))
        loadFile(action->data().toString());
}
