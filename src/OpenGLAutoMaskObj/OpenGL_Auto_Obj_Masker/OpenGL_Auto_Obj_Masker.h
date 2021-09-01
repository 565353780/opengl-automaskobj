#include <fstream>

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

#include "easymesh.h"
#include "EasyMesh2D.h"
#include "EasyMask2D.h"

using namespace GCL;

class OpenGL_Auto_Obj_Masker
{
public:
    OpenGL_Auto_Obj_Masker()
    {
    }

public:
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
        std::vector<float> &polygon);

    bool getMeshProjectRects(
        std::vector<std::vector<float>> &project_rect_vec);

    bool saveImageAndLabel(
        const QString &output_dataset_dir,
        const QString &image_basename,
        const QString &label_name,
        const size_t &current_label_idx_in_image,
        const QString &data_type);

    bool Create_Dataset(
        const QString &source_dataset_path,
        const QString &output_dataset_dir,
        const size_t &data_width,
        const size_t &data_height);

private:
    void setGLFormat();

    bool initEnv(
        const size_t &data_width,
        const size_t &data_height);

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

    bool getProjectPoint2D(
        const QMatrix4x4 &transform_matrix,
        const QVector3D &point_3d,
        std::vector<float> &point_2d);

    bool getProjectMesh2D(
        const EasyMesh *mesh,
        EasyMesh2D &mesh_2d);

    bool findConnectedFace(
        const EasyMesh2D &mesh_2d,
        const std::vector<int> current_search_vertex_idx_vec,
        EasyMesh2D &sub_mesh_2d,
        std::vector<bool> &vertex_connected_vec,
        std::vector<int> &sub_vertex_real_idx_vec);

    bool splitMesh2D(
        const EasyMesh2D &mesh_2d,
        std::vector<EasyMesh2D> &mesh_2d_vec);

    bool getPolygonVec(
        const EasyMesh2D &mesh_2d,
        std::vector<EasyPolygon2D> &polygon_vec);

    bool getPolygonVec(
        const std::vector<EasyMesh2D> &mesh_2d_vec,
        std::vector<EasyPolygon2D> &polygon_vec);

    bool getMeshProjectRect(
        const EasyMesh *mesh,
        std::vector<float> &project_rect);

    static QFileInfoList GetFileList(QString path);

    static bool cpDir(QString srcPath, QString dstPath);
    static bool delDir(QString dirName);

private:
    QRenderWidget w_;
    Q3DScene *scene_;
    QMaterial *material_;
    GLint viewport_[4];

    std::vector<EasyMesh *> mesh_list_;

    QFileInfo mesh_file_info_;
};
