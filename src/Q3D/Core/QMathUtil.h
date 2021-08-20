#ifndef QMATHUTIL_H
#define QMATHUTIL_H

#include <QVector3D>
#include <QMatrix4x4>
#include <QList>
#include "q3d_global.h"
#include "Core/Math/MathDefines.h"
namespace GCL {

class Q3DGCLSHARED_EXPORT QMathUtil {
public:

static QVector3D  getRayPlaneIntersect(const QVector3D& rayPos, const QVector3D &rayDir, const QVector3D &planePoint,
                              const QVector3D &planeNormal, bool *has_intersection = nullptr);

static QVector3D fromMatrixToEuler(const QMatrix4x4 &m,const QString &order = "ZYX");

static QVector3D mulEuler(const QVector3D &x, const QVector3D &y);
static float getDistanceSumToPlane(const QList<QVector3D> &vlist, const QVector3D &point, const QVector3D &normal);
static float getDistanceSquareSumToPlane(const QList<QVector3D> &vlist, const QVector3D &point, const QVector3D &normal);

static void computePCA(const QList<QVector3D> &vlist, QVector3D &axis_0, QVector3D &axis_1, QVector3D &axis_2);

static QVector3D getRayTriangleIntesect(const QVector3D& rayPos, const QVector3D &rayDir, const QVector3D &v0,
                                        const QVector3D &v1,const QVector3D &v2, bool *has_intersection = nullptr);




static QVector3D fromVectorTransformToEuler(const QVector3D &v0, const QVector3D &v1, const QString &order = "ZYX");
static QVector3D minValue(const QVector3D &v0, const QVector3D &v1)
{
    QVector3D ans;
    for(int i=0; i < 3; i++)
    {
        ans[i] = std::min(v0[i],v1[i]);
    }
    return ans;
}


static QVector3D maxValue(const QVector3D &v0, const QVector3D &v1)
{
    QVector3D ans;
    for(int i=0; i < 3; i++)
    {
        ans[i] = std::max(v0[i],v1[i]);
    }
    return ans;
}

static Vec3 toVec(const QVector3D &v)
{
    return Vec3(v.x(),v.y(),v.z());
}
static QVector3D toQVec(const Vec3 &v)
{
    return QVector3D(v[0],v[1],v[2]);
}
};

}
#endif // QMATHUTIL_H
