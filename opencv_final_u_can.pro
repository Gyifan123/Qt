#-------------------------------------------------
#
# Project created by QtCreator 2017-05-06T19:25:51
#
#-------------------------------------------------

QT       += core gui network

QT       += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = opencv_final_u_can
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
    qextserialbase.cpp \
    qextserialport.cpp \
    win_qextserialport.cpp

HEADERS  += mainwindow.h \
    qextserialbase.h \
    qextserialport.h \
    win_qextserialport.h

FORMS    += mainwindow.ui

INCLUDEPATH+= D:\senmonTools\OpenCV_31\opencv\build\include
D:\senmonTools\OpenCV_31\opencv\build\include\opencv
D:\senmonTools\OpenCV_31\opencv\build\include\opencv2

LIBS += -L D:\senmonTools\OpenCV_31\opencv\x86\mingw\lib\libopencv_*.a

RC_FILE = pic.rc

RESOURCES +=
