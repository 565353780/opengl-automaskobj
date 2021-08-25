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

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
typedef OpenMesh::PolyMesh_ArrayKernelT<>  MyOpenMesh;

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

class EasyVertex2D
{
public:
    EasyVertex2D()
    {
    }

    bool haveThisVertexNeighboor(
        const int &neighboor_vertex_idx);

    bool haveThisFaceNeighboor(
        const int &neighboor_face_idx);

    bool addVertexNeighboor(
        const int &neighboor_vertex_idx);

    bool addFaceNeighboor(
        const int &neighboor_face_idx);

    int x;
    int y;
    std::vector<int> neighboor_vertex_idx_vec;
    std::vector<int> neighboor_face_idx_vec;
};

class EasyFace2D
{
public:
    EasyFace2D()
    {
    }
    
    bool setVertexIndex(
        const int &vertex_idx_1,
        const int &vertex_idx_2,
        const int &vertex_idx_3);

    bool haveThisVertex(
        const int &vertex_idx);

    std::vector<int> vertex_idx_vec;
};

class EasyMesh2D
{
public:
    EasyMesh2D()
    {
    }

    bool addVertex(
        const int &x,
        const int &y);

    bool addVertexNeighboor(
        const int &vertex_1_idx,
        const int &vertex_2_idx);

    bool addFace(
        const int &vertex_idx_1,
        const int &vertex_idx_2,
        const int &vertex_idx_3);

    int first_vertex_idx=0;
    std::vector<EasyVertex2D> vertex_2d_list;
    std::vector<EasyFace2D> face_2d_list;
};

class EasyPoint2D
{
public:
    EasyPoint2D()
    {
    }

    int x;
    int y;
};

class EasyPolygon
{
public:
    EasyPolygon()
    {
    }

    std::vector<EasyPoint2D> point_list;
};

class OpenGL_Auto_Obj_Masker
{
public:
    OpenGL_Auto_Obj_Masker()
    {
    }

public:
    int testOffScreen(QString filename);

    bool initEnv();

    bool addNormalizedMesh(
        const QString &mesh_file_path,
        const QVector3D &center,
        const QVector3D &eular,
        const int &label_idx);

    bool clearMesh();

    bool getMeshRect3D(
        const EasyMesh *mesh,
        std::vector<float> &rect_3d);

    bool setMeshPose(
        EasyMesh *mesh,
        const QVector3D &center,
        const QVector3D &eular);

    bool getMeshProjectPolygon(
        const EasyMesh *mesh,
        std::vector<int> &polygon);

    bool getMeshProjectRects(
        std::vector<std::vector<int>> &project_rect_vec);

    bool saveImageAndLabel(
        const QString &output_name);

    bool Create_Dataset();

private:
    void setGLFormat();

    bool normalizeMesh(
        EasyMesh *mesh);

    bool transformMesh(
        EasyMesh *mesh,
        const QVector3D &center,
        const QVector3D &eular);

    EasyMesh *createMesh(
        const QString &mesh_file_path,
        const int &label_idx);

    EasyMesh *createNormalizedMeshWithPose(
        const QString &mesh_file_path,
        const QVector3D &center,
        const QVector3D &eular,
        const int &label_idx);

    bool createOpenMesh(
        const EasyMesh *mesh,
        MyOpenMesh &open_mesh);

    bool saveOpenMesh(
        const MyOpenMesh &open_mesh,
        const QString &output_file_path);

    bool getProjectPoint2D(
        const QMatrix4x4 &transform_matrix,
        const QVector3D &point_3d,
        std::vector<float> &point_2d);

    bool getProjectMesh2D(
        const EasyMesh *mesh,
        EasyMesh2D &mesh_2d);

    bool findConnectedVertex(
        const EasyMesh2D &mesh_2d,
        const std::vector<int> current_search_vertex_idx_vec,
        EasyMesh2D &sub_mesh_2d,
        std::vector<bool> &vertex_connected_vec,
        std::vector<int> &sub_vertex_real_idx_vec);

    bool splitMesh2D(
        const EasyMesh2D &mesh_2d,
        std::vector<EasyMesh2D> &mesh_2d_vec);

    bool getPolygon(
        const EasyMesh2D &mesh_2d,
        EasyPolygon &polygon);

    bool getPolygonVec(
        const std::vector<EasyMesh2D> &mesh_2d_vec,
        std::vector<EasyPolygon> &polygon_vec);

    bool getMeshProjectRect(
        const EasyMesh *mesh,
        std::vector<int> &project_rect);

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

    QFileInfo mesh_file_info_;
};
