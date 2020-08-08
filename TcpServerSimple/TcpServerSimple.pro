#-------------------------------------------------
#
# Project created by QtCreator 2017-10-07T14:42:57
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TcpServerSimple
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    Network/SocketServer.cpp

HEADERS  += MainWindow.h \
    Network/SocketConstant.h \
    Network/SocketServer.h \
    Network/constant.h

FORMS    += MainWindow.ui

RESOURCES += \
    pic.qrc
