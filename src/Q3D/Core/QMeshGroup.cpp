#include "QMeshGroup.h"
#include <QRegularExpression>
namespace GCL {

QMeshGroup::QMeshGroup(QObject *parent)
{
    this->initializeOpenGLFunctions();
}

void QMeshGroup::init()
{

}

void QMeshGroup::render(const QMatrix4x4 &project_matrix, const QMatrix4x4 &model_matrix)
{
    this->transform_matrix_.setToIdentity();
    this->transform_matrix_.translate(offset_);
    this->transform_matrix_.rotate(rotate_euler_[0],QVector3D(1,0,0));
    this->transform_matrix_.rotate(rotate_euler_[1],QVector3D(0,1,0));
    this->transform_matrix_.rotate(rotate_euler_[2],QVector3D(0,0,1));

    QMatrix4x4 _modelmatrix = model_matrix * this->transform_matrix_;
    QRegularExpression re;

    QList<QMesh3D*> models = this->findChildren<QMesh3D*>(re,Qt::FindDirectChildrenOnly);
    foreach (QMesh3D *model, models) {
        if(!model->isVisible()) continue;
        model->render(project_matrix,_modelmatrix);
    }

}

}
