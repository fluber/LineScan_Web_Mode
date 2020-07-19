QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    listenthread.cpp \
    main.cpp \
    mainwindow.cpp \
    saveimagethread.cpp

HEADERS += \
    listenthread.h \
    mainwindow.h \
    saveimagethread.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

message($$QMAKESPEC)

INCLUDEPATH += "C:\Program Files (x86)\Euresys\MultiCam\Include"
INCLUDEPATH += "C:\Program Files (x86)\Basler\clBBProtocolLib\include"
#win32-msvc: {
#LIBS += -L"C:\Program Files (x86)\Euresys\MultiCam\Lib\amd64" -lMultiCam
#LIBS += -L"C:\Program Files (x86)\Basler\clBBProtocolLib\lib" -lClBBProtocolLib_w64 -lclallserial_w64
#}
#win32-g++: {
#LIBS += -L"C:\Program Files (x86)\Euresys\MultiCam\Lib" -lMultiCam
#LIBS += -L"C:\Program Files (x86)\Basler\clBBProtocolLib\lib" -lClBBProtocolLib -lclallserial
#}
LIBS += -L"C:\Program Files (x86)\Euresys\MultiCam\Lib" -lMultiCam
LIBS += -L"C:\Program Files (x86)\Basler\clBBProtocolLib\lib" -lClBBProtocolLib -lclallserial
