QT += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/DataRebuild.cpp

HEADERS += \
    include/mainwindow.h \
    include/DataRebuild.h

FORMS += \
    forms/mainwindow.ui

TRANSLATIONS += \
    OpenGLAutoMaskObj_zh_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

