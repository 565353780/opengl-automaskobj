#ifndef POINTMAPMODELFINDER_H
#define POINTMAPMODELFINDER_H
#include "../q3d_global.h"
#include <QObject>
#include <QOpenGLFunctions>
#include <QList>
#include <QQuaternion>
#include <QVector3D>
namespace GCL {
class QMesh3D;
class Q3DScene;
class Q3DGCLSHARED_EXPORT QPointMapModelFinder : public QObject,protected QOpenGLFunctions
{
    Q_OBJECT
public:
    struct Pose3D{
      QQuaternion  quat_;
      QVector3D    offset_;
      qreal score_{0.0};
    };
    QPointMapModelFinder();

public:
    void renderToScreen(); // for debug

    double find(Pose3D startPose = Pose3D());


    QList<Pose3D> results() const;

protected:
    Q3DScene *scene_{nullptr};
    QMesh3D *point_map_{nullptr};
    QMesh3D *mesh_{nullptr};
    QList<Pose3D> results_;
};
}
#endif // POINTMAPMODELFINDER_H
