#include <iostream>
#include <stdlib.h>
#include <QString>
#include <QDir>
#include <QDebug>

#include <QApplication>
#include <QLabel>
#include <QSurfaceFormat>

#include <QOpenGLContext>
#include <QOffscreenSurface>
#include <QWidget>
#include <QOpenGLFramebufferObject>
#include <QImage>
#include <ctime>
#include <QFileInfo>

#ifdef WIN32
#include <gl/GLU.h>
#endif
#ifdef Linux
#include <GL/glu.h>
#endif
#include <iostream>
#include <QtCore>
#include <fstream>
#include <QOpenGLBuffer>

class DataRebuild
{
public:
    DataRebuild();
    ~DataRebuild();

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
    void saveImgAndJson(QString output_name, QRenderWidget &w, std::vector<EasyMesh *> easymesh_list, Q3DScene *scene, GLint *viewport);
    bool Create_Dataset(int create_data_num, int max_obj_num_per_img);

public:
    int class_num;
};
