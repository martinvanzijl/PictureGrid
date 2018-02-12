#-------------------------------------------------
#
# Project created by QtCreator 2018-02-12T12:56:31
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
qtHaveModule(printsupport): QT += printsupport

TARGET = PictureGrid
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp\
	imageviewer.cpp

HEADERS  += mainwindow.h\
	imageviewer.h

FORMS    += mainwindow.ui
