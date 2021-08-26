QT += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

win32{
DESTDIR = ../../bin_win
}
unix{
DESTDIR = ../../bin_linux
}

CONFIG += c++17

DEFINES += QT_DEPRECATED_WARNINGS

# OpenGL_Auto_Obj_Masker
SOURCES += \
    OpenGL_Auto_Obj_Masker/OpenGL_Auto_Obj_Masker.cpp \
    OpenGL_Auto_Obj_Masker/PointMapWidget.cpp \
    OpenGL_Auto_Obj_Masker/easymesh.cpp \
    OpenGL_Auto_Obj_Masker/EasyMesh2D.cpp \
    OpenGL_Auto_Obj_Masker/EasyPolygon.cpp

HEADERS += \
    OpenGL_Auto_Obj_Masker/OpenGL_Auto_Obj_Masker.h \
    OpenGL_Auto_Obj_Masker/PointMapWidget.h \
    OpenGL_Auto_Obj_Masker/easymesh.h \
    OpenGL_Auto_Obj_Masker/EasyMesh2D.h \
    OpenGL_Auto_Obj_Masker/EasyPolygon.h

# Q3D
INCLUDEPATH += $$PWD/../Q3D

DEPENDPATH += $$PWD/../Q3D

win32{
LIBS += \
    -L$$PWD/../../bin_win/q3dlib/ \
    -lq3d_gcl
}

unix{
LIBS += \
    -L$$PWD/../../bin_linux/q3dlib/ \
    -lq3d_gcl
}

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    OpenGLAutoMaskObj_zh_CN.ts

win32{
LIBS += -lopengl32 -lglu32
}

unix{
LIBS += -lglut -lGLU
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

