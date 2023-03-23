#-------------------------------------------------
#
# Project created by QtCreator 2023-01-22T14:41:15
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = stomatology
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

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    database.cpp \
    reg_window.cpp \
    reviews_window.cpp \
    record_window.cpp \
    dialogforreview.cpp \
    timeedit.cpp

HEADERS += \
        mainwindow.h \
    database.h \
    reg_window.h \
    reviews_window.h \
    record_window.h \
    dialogforreview.h \
    timeedit.h

FORMS += \
        mainwindow.ui \
    reg_window.ui \
    reviews_window.ui \
    record_window.ui \
    dialogforreview.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
