#-------------------------------------------------
#
# Project created by QtCreator 2018-04-11T18:30:18
#
#-------------------------------------------------

QT       += core gui network multimedia websockets printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ctips
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
        httphandler.cpp \
        messageevent.cpp \
        gamehelper.cpp \
        gamesplash.cpp

HEADERS += \
        mainwindow.h \
        httphandler.h \
        messageevent.h \
        gamehelper.h \
        gamesplash.h

FORMS += \
        mainwindow.ui

DISTFILES += \
    Resource/message.wav

macx {
    ICON = Resources/blue-icon.icns
}

include(./QtWebApp/httpserver/httpserver.pri)

RESOURCES += \
    resources.qrc
