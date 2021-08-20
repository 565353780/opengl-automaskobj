QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

win32{
DESTDIR = ../../bin_win
}
unix{
DESTDIR = ../../bin_linux
}

CONFIG += c++17

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/DataRebuild.cpp

HEADERS += \
    include/mainwindow.h \
    include/DataRebuild.h

INCLUDEPATH += $$PWD/../Q3D

DEPENDPATH += $$PWD/../Q3D

FORMS += \
    forms/mainwindow.ui

TRANSLATIONS += \
    OpenGLAutoMaskObj_zh_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
