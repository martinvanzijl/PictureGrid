#include "mainwindow.h"
//#include "imageviewer.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QSettings>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QGuiApplication::setApplicationDisplayName(MainWindow::tr("PictureGrid"));
    QCommandLineParser commandLineParser;
    commandLineParser.addHelpOption();
    commandLineParser.addPositionalArgument(MainWindow::tr("[file]"), MainWindow::tr("Image file to open."));
    commandLineParser.process(QCoreApplication::arguments());

    // Specify settings.
    QCoreApplication::setOrganizationName("Martin van Zijl");
    QCoreApplication::setOrganizationDomain("martinvz.com");
    QCoreApplication::setApplicationName("PictureGrid");

    //ImageViewer imageViewer;
    MainWindow imageViewer;

    if (!commandLineParser.positionalArguments().isEmpty()
        && !imageViewer.loadFile(commandLineParser.positionalArguments().front())) {
        return -1;
    }

    imageViewer.centerOnScreen();
    imageViewer.show();

    return app.exec();
}
