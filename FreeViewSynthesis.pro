#-------------------------------------------------
#
# Project created by QtCreator 2019-09-12T09:48:55
#
#-------------------------------------------------

QT       += core gui

INCLUDEPATH += E:\opencv3.4\opencv-build\install\include\

INCLUDEPATH += E:\opencv3.4\opencv-build\install\include\opencv\

INCLUDEPATH += E:\opencv3.4\opencv-build\install\include\opencv2\

LIBS += E:\opencv3.4\opencv-build\lib\libopencv_*.a

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FreeViewSynthesis
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    loaddata.cpp \
    computeview.cpp

HEADERS  += mainwindow.h \
    loaddata.h \
    computeview.h

FORMS    += mainwindow.ui
