#ifndef QMODELFINDERWIDGET_H
#define QMODELFINDERWIDGET_H

#include "QRenderWidget.h"
#include <QVector3D>
#include <QVector2D>
#include "../Q3D/Finder/QModelFinder.h"
class QOpenGLFramebufferObject;
namespace GCL {
class QPointMapMesh;
class QMesh3D;
class QShaderEvaluator;
class Q3DGCLSHARED_EXPORT QModelFinderWidget : public QRenderWidget
{
public:

    using ResultTransform = QModelFinder::ResultTransform;
    QModelFinderWidget(QWidget *parent = nullptr);
    ~QModelFinderWidget();

    void setPointMap(int w, int h, const std::vector<float> &points, const std::vector<uchar> &colors);
    void loadPointMapPLY(const QString &filename, int w = 1024, int h = 819, bool zflip = false, float flip_z = 0.0);
    void loadModel(const QString &filename);
    void loadConfig(const QString &filename);


    QList<ResultTransform> getResults() const;
public:

    void clearResults();
public:
    void setScoreThreshold(const qreal &score_level);
    void setDepthRange(float max_depth = 1000.0, float min_depth = 0.0);
    void setBoundaryExtendLevel(int level);
    void setPartLeastSize(int size);
    void setInitOffset(QVector3D v);
protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);



protected:
    void findModels();
    void quickFindModels();
    void getSamplePoints( QList<QVector3D> &sample_points);
    double findModel();

    bool need_update_buffer_{false};

    int u_boundary_level_{6};
    int u_part_least_size_{30};

    float u_max_depth_{1000.0};
    float u_min_depth_{0.0};
    qreal score_level_{1.2};
    QVector2D s_pos_;
    QList< QModelFinder::ResultTransform > results_;

    QModelFinder *finder_{nullptr};

};
}
#endif // QMODELFINDERWIDGET_H
