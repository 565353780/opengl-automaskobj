#include <iostream>
#include <fstream>
#include <ctime>
#include <stdlib.h>

#include <QApplication>
#include <QWidget>
#include <QtCore>
#include <QString>
#include <QDir>
#include <QDebug>
#include <QImage>
#include <QLabel>
#include <QFileInfo>
#include <QSurfaceFormat>
#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLFramebufferObject>
#include <QOffscreenSurface>

#ifdef WIN32
#include <gl/GLU.h>
#endif
#ifdef Linux
#include <GL/glu.h>
#endif

#include "../Q3D/Core/Q3DScene.h"
#include "../Q3D/Meshes/QCubeMesh.h"
#include "../Q3D/Core/QMaterial.h"
#include "../Q3D/Widgets/QRenderWidget.h"
#include "../Q3D/Meshes/QQuadMesh.h"
#include "../Q3D/Meshes/QPointMapMesh.h"
#include "../Q3D/Evaluator/QShaderEvaluator.h"
#include "../Q3D/Widgets/QModelFinderWidget.h"
#include "../Q3D/Finder/QPointMapModelFinder.h"

#include "OpenGL_Auto_Obj_Masker/PointMapWidget.h"
#include "OpenGL_Auto_Obj_Masker/easymesh.h"

class OpenGL_Auto_Obj_Masker
{
public:
    OpenGL_Auto_Obj_Masker();
    ~OpenGL_Auto_Obj_Masker();

public:
    void loadPointClound(const QString &filename, int &w, int &h, std::vector<float> &points, std::vector<uchar> &colors);
    void testEvaluator();
    int testOnScreen();
    int testOffScreen(QString filename);
    void setGLFormat();
    std::vector<float> getRect3D(QMesh3D *mesh);
    std::vector<float> getEasyRect3D(EasyMesh *mesh);
    bool normalizeMesh(QMesh3D *mesh);
    bool normalizeEasyMesh(EasyMesh *mesh);
    std::vector<int> createMesh(QMaterial *material, Q3DScene *scene, QVector3D center, QVector3D eular, GLint *viewport);
    QMesh3D *createMesh(QString mesh_name, QMaterial *material, Q3DScene *scene);
    EasyMesh *createEasyMesh(QString mesh_name, QMaterial *material, Q3DScene *scene, int label_idx);
    bool transformMesh(QMesh3D *mesh, QVector3D center, QVector3D eular);
    bool transformEasyMesh(EasyMesh *mesh, QVector3D center, QVector3D eular);
    bool setMeshpose(QMesh3D *mesh, QVector3D center, QVector3D eular);
    bool setEasyMeshpose(EasyMesh *mesh, QVector3D center, QVector3D eular);
    std::vector<int> getMeshProjectRect(QMesh3D *mesh, Q3DScene *scene, GLint *viewport);
    std::vector<int> getEasyMeshProjectRect(EasyMesh *mesh, Q3DScene *scene, GLint *viewport);
    std::vector<std::vector<int>> getEasyMeshProjectRects(std::vector<EasyMesh *> mesh_list, Q3DScene *scene, GLint *viewport);
    QMesh3D *getNewMeshRect3D(QString mesh_name, QMaterial *material, Q3DScene *scene, QVector3D center, QVector3D eular, GLint *viewport);
    EasyMesh *getNewEasyMeshRect3D(QString mesh_name, QMaterial *material, Q3DScene *scene, QVector3D center, QVector3D eular, GLint *viewport, int label_idx);
    void saveImageAndLabel(QString output_name, QRenderWidget &w, std::vector<EasyMesh *> easymesh_list, Q3DScene *scene, GLint *viewport);
    bool Create_Dataset(int create_data_num, int max_obj_num_per_img);

private:
    static QFileInfoList GetFileList(QString path);
    static bool cpDir(QString srcPath, QString dstPath);
    static bool delDir(QString dirName);

public:
    int class_num_;
};
