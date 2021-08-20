#ifndef QICPALIGN_H
#define QICPALIGN_H
#include <QObject>
#include <QOpenGLFunctions>
#include <QQuaternion>
#include <QVector3D>
#include <QMatrix4x4>
#include <vector>
#include "../q3d_global.h"
#include "Math/MathDefines.h"

namespace GCL {


class Q3DGCLSHARED_EXPORT QICPAlign
{
public:
    struct PtPair {
        Vec3 p1, n1, p2, n2;
        PtPair(const Vec3 &p1_, const Vec3 &n1_,
               const Vec3 &p2_, const Vec3 &n2_) :
                p1(p1_), n1(n1_), p2(p2_), n2(n2_)
            {}

    };
    QICPAlign();

    static void align_symm(const std::vector<PtPair> &pairs,
                      const Vec3 &centroid1, const Vec3 &centroid2,
                       QMatrix4x4 &x_matrix);
};
}
#endif // QICPALIGN_H
