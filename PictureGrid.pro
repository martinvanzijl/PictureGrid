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
    imagelabel.cpp \
    percentvalidator.cpp \
    clickablelabel.cpp \
    imagescrollarea.cpp

HEADERS  += mainwindow.h\
    imagelabel.h \
    clickablelabel.h \
    imagescrollarea.h

FORMS    += mainwindow.ui

RESOURCES += \
    resources.qrc
