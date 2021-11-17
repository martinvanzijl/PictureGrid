#include "helpwindow.h"
#include "ui_helpwindow.h"

HelpWindow::HelpWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HelpWindow)
{
    ui->setupUi(this);

    ui->textBrowserMain->setSource(QUrl("qrc:/root/html/manual.html"));
}

HelpWindow::~HelpWindow()
{
    delete ui;
}
