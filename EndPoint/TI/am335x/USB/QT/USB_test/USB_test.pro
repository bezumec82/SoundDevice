#-------------------------------------------------
#
# Project created by QtCreator 2019-04-26T15:46:42
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = USB_test
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    lirausb/liraProtocol.cpp \
    lirausb/usbEnum.cpp

HEADERS += \
    lirausb/liraProtocol.h \
    lirausb/usbEnum.h \
    main.hpp \
    ../../../../workspace/LIRA_cpp/src/g711.h
