#-------------------------------------------------
#
# Project created by QtCreator 2016-10-08T10:16:38
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = KAMEditor
TEMPLATE = app

CONFIG += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    machinetool.cpp \
    commandinterpreter.cpp \
    addpointdialog.cpp \
    mousetoselectionpointdialog.cpp

HEADERS  += mainwindow.h \
    machinetool.h \
    commandinterpreter.h \
    command.h \
    vector.h \
    point.h \
    addpointdialog.h \
    mousetoselectionpointdialog.h

FORMS    += mainwindow.ui \
    addpointdialog.ui \
    mousetoselectionpointdialog.ui

win32:RC_FILE = application.rc
