#include "QModelFinderWidget.h"
#include "Meshes/QPointMapMesh.h"
#include "Core/Q3DScene.h"
#include "Evaluator/QShaderEvaluator.h"
#include <QKeyEvent>
#include <QOpenGLFramebufferObject>
#include "Core/QMaterial.h"
#include <QDebug>
#include <QMouseEvent>
#include <iostream>
#include <ctime>
#include <QPainter>
#include "Finder/QModelFinder.h"

#include <QFileDialog>
namespace GCL
{

QModelFinderWidget::QModelFinderWidget(QWidget *parent): QRenderWidget(parent)
{
    scene_->setRotationSpeed(2);
    scene_->setPanSpeed(40);
    scene_->setScaleSpeed(1);
    scene_->setProjectionParameters(0.1, 10000.0, 45);
    scene_->setManipulatorScaleMode(1);
    finder_ = new QModelFinder(scene_, this);
    this->show();
}

QModelFinderWidget::~QModelFinderWidget()
{

}

void QModelFinderWidget::setPointMap(int w, int h, const std::vector<float> &points, const std::vector<uchar> &colors)
{
    makeCurrent();
    finder_->setPointMap(w, h, points, colors);
}

void QModelFinderWidget::loadPointMapPLY(const QString &filename, int w, int h, bool zflip, float flip_z)
{
    makeCurrent();
    finder_->loadPointMapPLY(filename, w, h, zflip, flip_z);

}

void QModelFinderWidget::loadModel(const QString &filename)
{
    makeCurrent();
    finder_->loadModel(filename);
}

void QModelFinderWidget::loadConfig(const QString &filename)
{
    makeCurrent();
    finder_->loadConfig(filename);
}

QList<QModelFinderWidget::ResultTransform> QModelFinderWidget::getResults() const
{
    return finder_->getResults();
}

void QModelFinderWidget::clearResults()
{
    finder_->clearResults();
}

void QModelFinderWidget::initializeGL()
{
    QRenderWidget::initializeGL();
    finder_->init();


}

void QModelFinderWidget::resizeGL(int w, int h)
{

    QRenderWidget::resizeGL(w, h);
    makeCurrent();

    finder_->resize(w, h);
}

void QModelFinderWidget::paintGL()
{
    if(!finder_) return;
    makeCurrent();
    QOpenGLFramebufferObject::bindDefault();
    finder_->render();
}

void QModelFinderWidget::keyPressEvent(QKeyEvent *event)
{
    int offset = 20;

    QMesh3D *mesh = finder_->getMesh();
    if(event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_O)
    {

        QString filename = QFileDialog::getOpenFileName(0, tr("Open pointmap ply..."), "./", "*.ply");
        if(filename.isEmpty()) return;
        this->makeCurrent();
        finder_->loadPointMapPLY(filename);
        QOpenGLFramebufferObject::bindDefault();

        return;
    }
    else  if(event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_L)
    {
        QString filename = QFileDialog::getOpenFileName(0, tr("Open config json..."), "./", "*.json");
        if(filename.isEmpty()) return;
        this->makeCurrent();
        finder_->loadConfig(filename);
        QOpenGLFramebufferObject::bindDefault();
    }
    if(event->key() == Qt::Key_W)
    {
        mesh->rotateEuler(QVector3D(offset, 0, 0));
    }
    else if(event->key() == Qt::Key_S)
    {
        mesh->rotateEuler(QVector3D(-offset, 0, 0));
    }
    else if(event->key() == Qt::Key_A)
    {
        mesh->rotateEuler(QVector3D(0, offset, 0));
    }
    else if(event->key() == Qt::Key_D)
    {
        mesh->rotateEuler(QVector3D(0, -offset, 0));
    }
    else if(event->key() == Qt::Key_Q)
    {
        mesh->rotateEuler(QVector3D(0, 0, offset));
    }
    else if(event->key() == Qt::Key_E)
    {
        mesh->rotateEuler(QVector3D(0, 0, -offset));
    }
    else if(event->key() == Qt::Key_Left)
    {
        mesh->translate(QVector3D(offset, 0, 0));

    }
    else if(event->key() == Qt::Key_Right)
    {
        mesh->translate(QVector3D(-offset, 0, 0));
    }
    else if(event->key() == Qt::Key_Up)
    {
        mesh->translate(QVector3D(0, offset, 0));
    }
    else if(event->key() == Qt::Key_Down)
    {
        mesh->translate(QVector3D(0, -offset, 0));
    }

    else if(event->key() == Qt::Key_9)
    {
        mesh->translate(QVector3D(0, 0, offset / 2));
    }
    else if(event->key() == Qt::Key_0)
    {
        mesh->translate(QVector3D(0, 0, -offset / 2));
    }
    else if(event->key() == Qt::Key_Space)
    {
        getScene()->setDefaultView();
    }
    else if(event->key() == Qt::Key_1)
    {
        QVector3D eular =  mesh->getEuler();
        mesh->rotateEuler(QVector3D(90, 0, 0) - eular);
    }
    else if(event->key() == Qt::Key_2)
    {
        QVector3D eular =  mesh->getEuler();
        mesh->rotateEuler(QVector3D(0, 90, 0) - eular);
    }
    else if(event->key() == Qt::Key_5)
    {
        finder_->setViewByPointsCenter();
        update();
    }
    else if(event->key() == Qt::Key_L)
    {
        qDebug() << "reload Config";
        makeCurrent();
        finder_->reloadConfig();
    }
    else if(event->key() == Qt::Key_X)
    {
//        findAllModels();

        finder_->clearResults();

        results_.clear();
    }
    else if(event->key() == Qt::Key_R)
    {
        qDebug() << "score: " << findModel();
    }
    else if(event->key() == Qt::Key_I)
    {
        qDebug() << getScene()->getModelMatrix();
    }
    else if(event->key() == Qt::Key_T)
    {

    }
    else if(event->key() == Qt::Key_P)
    {
        qDebug() << mesh->getEuler();
    }
    else if(event->key() == Qt::Key_M)
    {
        findModels();
    }
    else if(event->key() == Qt::Key_N)
    {
        quickFindModels();
    }
    else if( event->key() == Qt::Key_B)
    {
        finder_->setIsRenderResult(!finder_->isRenderResult());
    }
    update();
}

void QModelFinderWidget::mousePressEvent(QMouseEvent *event)
{
    QRenderWidget::mousePressEvent(event);

    QMesh3D *mesh = finder_->getMesh();

    if(event->buttons() == Qt::LeftButton)
    {

        makeCurrent();
        clock_t clock_0 = clock();
        s_pos_ = QVector2D(event->x() / qreal(this->width()), 1.0 - event->y() / qreal(this->height()));
        QVector3D pos = finder_->getClickPos(s_pos_);
        mesh->setOffset(pos + finder_->getInitOffset());
        mesh->setEuler(QVector3D(0, 0, 0));
        qDebug() << pos;
        double mval = findModel();
        double score = mval;
        qDebug() << "score: " << score;
        qDebug() << (clock() - clock_0) / (CLOCKS_PER_SEC / 1000) << "ms";
        update();
    }
}

void QModelFinderWidget::findModels()
{
    makeCurrent();
//    finder_->setDebugMode(true);
    finder_->findModels();
    finder_->setDebugMode(false);
}


void QModelFinderWidget::quickFindModels()
{

    makeCurrent();
    finder_->setDebugMode(true);
    finder_-> quickFindAllModels();
    finder_->setDebugMode(false);

}

void QModelFinderWidget::getSamplePoints(QList<QVector3D> &sample_points)
{
    makeCurrent();
    finder_->getSamplePoints(sample_points);

}


double QModelFinderWidget::findModel()
{
    this->makeCurrent();
    return finder_->localSearching(s_pos_);
}



void QModelFinderWidget::setScoreThreshold(const qreal &score_level)
{
    score_level_ = score_level;
    finder_->setScoreThreshold(score_level);

}

void QModelFinderWidget::setDepthRange(float max_depth, float min_depth)
{
    u_max_depth_ = max_depth;
    u_min_depth_ = min_depth;
    finder_->setDepthRange(max_depth, min_depth);
}

void QModelFinderWidget::setBoundaryExtendLevel(int level)
{
    u_boundary_level_ = level;
    finder_->setBoundaryExtendLevel(level);
}

void QModelFinderWidget::setPartLeastSize(int size)
{
    u_part_least_size_ = size;
    finder_->setPartLeastSize(size);
}

void QModelFinderWidget::setInitOffset(QVector3D v)
{
    finder_->setInitOffset(v);
}


}
