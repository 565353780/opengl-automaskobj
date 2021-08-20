#include "PointMapModelFinder.h"
#include <QGraphicsView>
namespace GCL {

QPointMapModelFinder::QPointMapModelFinder()
{

}

void QPointMapModelFinder::renderToScreen()
{

}

double QPointMapModelFinder::find(Pose3D startPose)
{
    return 0.0;
}

QList<QPointMapModelFinder::Pose3D> QPointMapModelFinder::results() const
{
    return results_;
}
}
