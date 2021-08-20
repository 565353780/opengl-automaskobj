#include "QMathUtil.h"
#include <cmath>
//#include <Eigen/Dense>
#include <iostream>
namespace GCL {

QVector3D QMathUtil::getRayPlaneIntersect(const QVector3D &rayPos, const QVector3D &rayDir,
                              const QVector3D &planePoint, const QVector3D &planeNormal, bool *has_intersection)
{
    float t0 = QVector3D::dotProduct(rayDir,planeNormal);
    float t1 = QVector3D::dotProduct(rayPos - planePoint , planeNormal);
    if(fabs(t0) < 1e-7)
    {
        if(has_intersection)
        {
            *has_intersection = false;
        }
        return QVector3D(0,0,0);
    }

    float k = -t1 / t0;
    QVector3D intersect = rayPos + k * rayDir;
    if(has_intersection)
    {
        *has_intersection = true;
    }
    return intersect;
}
float clamp(float x, float l, float r)
{
    if(x < l) return l;
    if(x > r) return r;
    return x;
}
QVector3D QMathUtil::fromMatrixToEuler(const QMatrix4x4 &m, const QString &order)
{
    QMatrix3x3 tm = m.normalMatrix();

   float m11 = tm(0,0), m12 = tm(1,0), m13 = tm(2,0);
   float m21 = tm(0,1), m22 = tm(1,1), m23 = tm(2,1);
   float m31 = tm(0,2), m32 = tm(1,2), m33 = tm(2,2);
    float _x,_y,_z;
    if(order == "XYZ")
    {
        _y = asin(clamp(m13,-1,1));
        if(fabs(m13) < 0.99999)
        {
            _x = atan2(-m23,m33);
            _z = atan2(-m12,m11);
        }else
        {
            _x = atan2(m32,m22);
            _z = 0;
        }
    }else if ( order == "YXZ" ) {

        _x = asin( - clamp( m23, - 1, 1 ) );

        if ( fabs( m23 ) < 0.99999 ) {

            _y = atan2( m13, m33 );
            _z = atan2( m21, m22 );

        } else {

            _y = atan2( - m31, m11 );
            _z = 0;

        }

    } else if ( order == "ZXY" ) {

        _x = asin( clamp( m32, - 1, 1 ) );

        if ( fabs( m32 ) < 0.99999 ) {

            _y = atan2( - m31, m33 );
            _z = atan2( - m12, m22 );

        } else {

            _y = 0;
            _z = atan2( m21, m11 );

        }

    } else if ( order == "ZYX" ) {

        _y = asin( - clamp( m31, - 1, 1 ) );

        if ( fabs( m31 ) < 0.99999 ) {

            _x = atan2( m32, m33 );
            _z = atan2( m21, m11 );

        } else {

            _x = 0;
            _z = atan2( - m12, m22 );

        }

    } else if ( order == "YZX" ) {

        _z = asin( clamp( m21, - 1, 1 ) );

        if ( fabs( m21 ) < 0.99999 ) {

            _x = atan2( - m23, m22 );
            _y = atan2( - m31, m11 );

        } else {

            _x = 0;
            _y = atan2( m13, m33 );

        }

    } else if ( order == "XZY" ) {

        _z = asin( - clamp( m12, - 1, 1 ) );

        if ( fabs( m12 ) < 0.99999 ) {

            _x = atan2( m32, m22 );
            _y = atan2( m13, m11 );

        } else {

            _x = atan2( - m23, m33 );
            _y = 0;

        }

    } else {

         qDebug()<<( "THREE.Euler: .setFromRotationMatrix() given unsupported order: " + order );
    }

    QVector3D v(_x,_y,_z);
    v = v / 3.1415926535898 * 180;
    return -v;


}

QVector3D QMathUtil::mulEuler(const QVector3D &x, const QVector3D &y)
{
    QMatrix4x4 mat;
    mat.rotate(x[0],QVector3D(1,0,0));
    mat.rotate(x[1],QVector3D(0,1,0));
    mat.rotate(x[2],QVector3D(0,0,1));

    mat.rotate(y[0],QVector3D(1,0,0));
    mat.rotate(y[1],QVector3D(0,1,0));
    mat.rotate(y[2],QVector3D(0,0,1));

    return QMathUtil::fromMatrixToEuler(mat);

}

float QMathUtil::getDistanceSumToPlane(const QList<QVector3D> &vlist, const QVector3D &point, const QVector3D &normal)
{
    float sum = 0;
    for(const auto & v : vlist)
    {
        sum += fabs(QVector3D::dotProduct(v-point,normal));
    }
    return sum;
}

float QMathUtil::getDistanceSquareSumToPlane(const QList<QVector3D> &vlist, const QVector3D &point, const QVector3D &normal)
{
    float sum = 0;
    for(const auto & v : vlist)
    {
        float val = fabs(QVector3D::dotProduct(v-point,normal));
        sum += val * val;
    }
    return sum;
}

void QMathUtil::computePCA(const QList<QVector3D> &vlist, QVector3D &axis_0, QVector3D &axis_1, QVector3D &axis_2)
{
//    QVector3D center;
//    for(const auto &v : vlist)
//    {
//        center += v;
//    }
//    center/= vlist.size();
//    Eigen::Matrix3d mat;
//    for(const auto &v : vlist)
//    {
//        Eigen::Vector3d ev;
//        QVector3D tv = v - center;
//        for(int j=0; j < 3; j++)
//        {
//            ev(j) = tv[j];
//        }
//        Eigen::Matrix3d tm = ev * ev.transpose();
//        mat += tm;
//    }
//    mat /= vlist.size();
//    Eigen::EigenSolver<Eigen::Matrix3d> solver;
//    solver.compute(mat);
//    auto eigen_v = solver.eigenvectors();
//    for(int j=0; j < 3; j++)
//    {
//        axis_0[j] = eigen_v.coeff(j,0).real();
//        axis_1[j] = eigen_v.coeff(j,1).real();
//        axis_2[j] = eigen_v.coeff(j,2).real();
//    }


}

QVector3D QMathUtil::getRayTriangleIntesect(const QVector3D &rayPos, const QVector3D &rayDir, const QVector3D &v0, const QVector3D &v1, const QVector3D &v2, bool *has_intersection)
{
    Vec3 vray(rayPos.x(),rayPos.y(),rayPos.z());
    Vec3 vdir(rayDir.x(),rayDir.y(),rayDir.z());
    Vec3 vv0(v0.x(),v0.y(),v0.z());
    Vec3 vv1(v1.x(),v1.y(),v1.z());
    Vec3 vv2(v2.x(),v2.y(),v2.z());
    Vec3 ans;
    bool t = Vec3::getIntersectionRayToTriangle(vray,vdir,vv0,vv1,vv2,ans);

    if(has_intersection)
    {
        *has_intersection = t;
    }
    return QVector3D(ans[0],ans[1],ans[2]);
}

QVector3D QMathUtil::fromVectorTransformToEuler(const QVector3D &v0, const QVector3D &v1,const QString &order)
{
    Vec3 vv0(v0.x(),v0.y(),v0.z());
    Vec3 vv1(v1.x(),v1.y(),v1.z());

    Quat quat =  Quat::quatFromVectorTransform(vv0,vv1);
    HomoMatrix4 hmat =  quat.convertToMatrix();

    QMatrix4x4 qmat(hmat.data(),4,4);

    return fromMatrixToEuler(qmat,order);

}



}
