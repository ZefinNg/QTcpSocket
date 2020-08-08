#-------------------------------------------------
#
# Project created by QtCreator 2017-10-07T15:40:28
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TcpClientSimple
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    Network/SocketClient.cpp

HEADERS  += MainWindow.h \
    Network/SocketClient.h \
    Network/SocketConstant.h \
    Constant.h

FORMS    += MainWindow.ui
