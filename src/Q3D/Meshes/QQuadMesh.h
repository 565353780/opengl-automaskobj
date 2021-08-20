#ifndef QQUADMESH_H
#define QQUADMESH_H
#include "../Core/QMesh3D.h"
namespace GCL {

class Q3DGCLSHARED_EXPORT QQuadMesh : public QMesh3D
{
public:
     QQuadMesh(QObject *parent = nullptr);
     QQuadMesh(QMaterial *material, QObject *parent = nullptr);
    void init() override;

    void setElementType(GLuint type);

    void updateRect(float x, float y, float w, float h);


};
}
#endif // QQUATMESH_H
