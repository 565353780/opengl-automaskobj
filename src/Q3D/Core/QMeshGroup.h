#ifndef QMESHGROUP_H
#define QMESHGROUP_H
#include "QMesh3D.h"
namespace GCL {

class Q3DGCLSHARED_EXPORT QMeshGroup : public QMesh3D
{
public:
   explicit QMeshGroup(QObject *parent = nullptr);

   void init();
   void render(const QMatrix4x4 &project_matrix = QMatrix4x4(),
                            const QMatrix4x4 &model_matrix = QMatrix4x4());

};
}
#endif // QMESHGROUP_H
