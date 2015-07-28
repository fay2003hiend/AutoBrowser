#-------------------------------------------------
#
# Project created by QtCreator 2015-07-28T12:30:20
#
#-------------------------------------------------

QT       += core gui webkit

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets webkitwidgets

TARGET = AutoBrowserDemo
TEMPLATE = app

CONFIG += console

SOURCES += main.cpp\
        widgetmain.cpp \
    autowebview.cpp

HEADERS  += widgetmain.h \
    autowebview.h

FORMS    += widgetmain.ui
