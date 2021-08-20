QT +=core gui widgets opengl
TEMPLATE = lib

win32{
DESTDIR = ../../bin_win/q3dlib
}
unix{
DESTDIR = ../../bin_linux/q3dlib
}

TARGET = q3d_gcl
RESOURCES += \
    q3d.qrc

DISTFILES +=

DEFINES += Q3D_GCL_LIBRARY

HEADERS += \
    q3d_global.h \
    Meshes/QCubeMesh.h \
    Meshes/QPointMapMesh.h \
    Meshes/QQuadMesh.h \
    Widgets/QRenderWidget.h \
    Core/Q3DScene.h \
    Core/QMaterial.h \
    Core/QMesh3D.h \
    Core/QMeshParser.h \
    Core/QICPAlign.h \
    Widgets/QModelFinderWidget.h \
    Evaluator/QShaderEvaluator.h \
    Meshes/QPrimitiveMesh.h \
    Core/QMathUtil.h \
    Core/Math/HomoMatrix.h \
    Core/Math/MathDefines.h \
    Core/Math/Quaternion.h \
    Core/Math/Vec.h \
    Finder/QFinderConfig.h \
    Finder/QModelFinder.h \
    Finder/QPointMapModelFinder.h \
    Meshes/QAxesMesh.h \
    Core/QMeshGroup.h \
    Widgets/QInputVector3Dialog.h \
    Widgets/QInputTransformDialog.h

SOURCES += \
    Meshes/QCubeMesh.cpp \
    Meshes/QPointMapMesh.cpp \
    Meshes/QQuadMesh.cpp \
    Widgets/QRenderWidget.cpp \
    Core/Q3DScene.cpp \
    Core/QMaterial.cpp \
    Core/QMesh3D.cpp \
    Core/QMeshParser.cpp \
    Core/QICPAlign.cpp \
    Widgets/QModelFinderWidget.cpp \
    Evaluator/QShaderEvaluator.cpp \
    Meshes/QPrimitiveMesh.cpp \
    Core/QMathUtil.cpp \
    Finder/QFinderConfig.cpp \
    Finder/QModelFinder.cpp \
    Finder/QPointMapModelFinder.cpp \
    Meshes/QAxesMesh.cpp \
    Core/QMeshGroup.cpp \
    Widgets/QInputVector3Dialog.cpp \
    Widgets/QInputTransformDialog.cpp

INCLUDEPATH += Core/ \
               $$PWD/../../thirdparty/  \
               $$PWD/../../thirdparty/trimesh2

#win32: LIBS += -L$$PWD/../../tifflib/ -llibtiff

#INCLUDEPATH += $$PWD/../../tifflib/inc
#DEPENDPATH += $$PWD/../../tifflib

FORMS += \
    Widgets/qinputvector3dialog.ui \
    Widgets/qinputtransformdialog.ui
