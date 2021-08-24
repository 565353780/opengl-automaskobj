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

using namespace GCL;

class OpenGL_Auto_Obj_Masker
{
public:
    OpenGL_Auto_Obj_Masker();
    ~OpenGL_Auto_Obj_Masker();

public:
    int testOffScreen(QString filename);

    bool initEnv();

    bool addNormalizedMesh(QString &mesh_file_path, QVector3D &center, QVector3D &eular, int &label_idx);

    bool clearMesh();

    bool getMeshRect3D(EasyMesh *mesh, std::vector<float> &rect_3d);

    bool setMeshPose(EasyMesh *mesh, QVector3D &center, QVector3D &eular);

    bool getMeshProjectPolygon(EasyMesh *mesh, std::vector<int> &polygon);

    bool getMeshProjectRects(std::vector<std::vector<int>> &project_rect_vec);

    void saveImageAndLabel(QString &output_name);

    bool Create_Dataset();

private:
    void setGLFormat();

    bool Point3DToPoint2D(QMatrix4x4 &transform_matrix, QVector3D &point_3d, std::vector<int> &point_2d);

    bool normalizeMesh(EasyMesh *mesh);

    bool transformMesh(EasyMesh *mesh, QVector3D &center, QVector3D &eular);

    EasyMesh *createMesh(QString &mesh_name, int &label_idx);

    EasyMesh *createNormalizedMeshWithPose(QString &mesh_name, QVector3D &center, QVector3D &eular, int &label_idx);

    bool getMeshProjectRect(EasyMesh *mesh, std::vector<int> &project_rect);

    static QFileInfoList GetFileList(QString path);

    static bool cpDir(QString srcPath, QString dstPath);
    static bool delDir(QString dirName);

public:
    int class_num_;

private:
    QRenderWidget w_;
    Q3DScene *scene_;
    QMaterial *material_;
    GLint viewport_[4];

    std::vector<EasyMesh *> mesh_list_;
};
