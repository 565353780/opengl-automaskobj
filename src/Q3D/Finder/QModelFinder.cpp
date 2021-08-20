#include "QModelFinder.h"
#include "Evaluator/QShaderEvaluator.h"
#include "Core/Q3DScene.h"
#include "Meshes/QPointMapMesh.h"
#include "Core/QMesh3D.h"
#include <QPainter>
#include <ctime>
#include <QOpenGLFramebufferObject>
#include "Core/QMaterial.h"
#include <algorithm>
#include <QOpenGLContext>
#include <QOffscreenSurface>
#include "QFinderConfig.h"
#include "Meshes/QAxesMesh.h"
#include "Core/QMathUtil.h"
#include "Core/QICPAlign.h"
namespace GCL
{

QModelFinder::QModelFinder(Q3DScene *scene, QObject *parent): QObject(parent), scene_(scene), config_(new QFinderConfig(this))
{
    if(!scene_)
    {
        scene_ = new Q3DScene(this);
    }

    scene_->setPackMinval(0.0);
}

QModelFinder::~QModelFinder()
{
    if(z_buffer_)
    {
        delete z_buffer_;
        z_buffer_ = nullptr;
    }

    if(normal_buffer_)
    {
        delete normal_buffer_;
        normal_buffer_ = nullptr;
    }

    if(tfbo_buffer_)
    {
        delete tfbo_buffer_;
        tfbo_buffer_ = nullptr;
    }

    this->destroy();
}

void QModelFinder::loadConfig(const QString &filename)
{
    makeCurrent();

    qDebug() << "Config filename:" << filename;
    QList<QString> modelnames = config_->modelnames();
    for(auto name : modelnames)
    {
        if(model_dict_[name])
        {
            model_dict_[name]->deleteLater();
            model_dict_[name] = nullptr;
        }
    }
    model_dict_.clear();


    config_->loadFile(filename);
    cfg_filename_ = filename;
    modelnames = config_->modelnames();
    model_dict_.clear();
    for(auto name : modelnames)
    {
        QString path = config_->getFilepath(name);

        QMesh3D *mesh = new QMesh3D(material_, scene_);
        mesh->setObjectName(name);
        mesh->loadFile(path, true);
        mesh->setOffset(QVector3D(0, 0, -9999));
        model_dict_[name] = mesh;


        mesh_ = mesh;

    }

    if(tfbo_buffer_)
    {
        if(tfbo_buffer_->isValid())
        {
            tfbo_buffer_->release();
        }
        delete tfbo_buffer_;
        tfbo_buffer_ = nullptr;
    }

    if(!tfbo_buffer_)
    {
        int tfbo_size = config_->getCanvasSize();
        tfbo_buffer_ = new QOpenGLFramebufferObject(tfbo_size, tfbo_size, QOpenGLFramebufferObject::CombinedDepthStencil);
    }


    this->u_trunc_ = config_->getDistTrunc();
    this->u_min_depth_ = config_->getMinDepth();
    this->u_max_depth_ = config_->getMaxDepth();
    this->u_score_threshold_ = config_->getScoreThreshold();
    this->u_boundary_level_ = config_->getBoundaryExtendLevel();
    this->u_initial_offset_ = config_->getInitOffset();
    this->u_part_least_size_ = config_->getPartLeastSize();
    this->u_local_iteration_num_ = config_->getInnerIterationNum();
    this->scene_->setPackMinval(config_->getPackMinval());
    this->scene_->setDefaultModelMatrix(config_->getDefaultViewMatrix());
    this->scene_->setDefaultView();
    this->is_debug_mode_ = config_->getDebugMode();
    this->resizeBuffer(config_->getBufferWidth(), config_->getBufferHeight());
    this->u_sample_points_.clear();
    this->updateBuffer();
}

void QModelFinder::reloadConfig()
{
    if(cfg_filename_.isEmpty()) return;
    loadConfig(cfg_filename_);
}

void QModelFinder::create()
{
    if(m_context_) return;
    if(m_surface_) return;

    QSurfaceFormat surfaceFmt;
    if(QOpenGLContext::openGLModuleType() == QOpenGLContext::LibGL)
    {
        surfaceFmt.setRenderableType(QSurfaceFormat::OpenGL);
    }
    else
    {
        surfaceFmt.setRenderableType(QSurfaceFormat::OpenGLES);
    }


    m_context_ = new QOpenGLContext();

    m_context_->setFormat(surfaceFmt);


    surfaceFmt.setDepthBufferSize(24);
    QSurfaceFormat::setDefaultFormat(surfaceFmt);


    m_context_->create();
    m_surface_ = new QOffscreenSurface();
    m_surface_->create();

    this->makeCurrent();
}

void QModelFinder::destroy()
{
    if(m_surface_)
    {
        m_surface_->deleteLater();
        m_surface_ = nullptr;


        if(m_context_)
        {
            m_context_->deleteLater();
            m_context_ = nullptr;
        }
    }

}

void QModelFinder::makeCurrent()
{
    if(m_context_)
    {
        m_context_->makeCurrent(m_surface_);
    }
}

void QModelFinder::doneCurrent()
{
    if(m_context_)
    {
        m_context_->doneCurrent();
    }
}

void QModelFinder::init()
{
    this->initializeOpenGLFunctions();
    this->makeCurrent();
    scene_->init();
    pointmap_ = new QPointMapMesh(scene_);
    pointmap_->setObjectName("pointmap");

    mesh_ = new QMesh3D(scene_);
    mesh_->setObjectName("findmodel");
    mesh_->initShader(":/shaders/findmodel_vshader.glsl", ":/shaders/findmodel_fshader.glsl");
    material_ = mesh_->getMaterial();
    evaluator_ = new QShaderEvaluator(this);
    axes_ = new QAxesMesh(this);
    axes_->setLength(10);
    axes_->setRadius(2);
    axes_->init();
}

void QModelFinder::resize(int w, int h)
{
    width_ = w;
    height_ = h;
    scene_->resize(w, h);
}

void QModelFinder::resizeBuffer(int w, int h)
{
    if(z_buffer_)
    {
        delete z_buffer_;
        z_buffer_ = nullptr;
    }

    if(normal_buffer_)
    {
        delete normal_buffer_;
        normal_buffer_ = nullptr;
    }

    z_buffer_ = new QOpenGLFramebufferObject(w, h, QOpenGLFramebufferObject::CombinedDepthStencil);
    normal_buffer_ = new QOpenGLFramebufferObject(w, h, QOpenGLFramebufferObject::CombinedDepthStencil);
    need_update_buffer_ = true;
    scene_->resize(w, h);

}

void QModelFinder::setPointMap(int w, int h, const std::vector<float> &points, const std::vector<uchar> &colors)
{
    if(pointmap_)
    {
        delete pointmap_;
        pointmap_ = new QPointMapMesh(scene_);
        pointmap_->setObjectName("pointmap");
    }
    pointmap_->setPointMap(w, h, points, colors);
    need_update_buffer_ = true;
}

void QModelFinder::loadPointMapPLY(const QString &filename, int w, int h, bool zflip, float flip_z)
{
    this->makeCurrent();
    if(w <= 0 || h <= 0)
    {
        w = config_->getPolymapWidth();
        h = config_->getPolymapHeight();
    }
    if(pointmap_)
    {
        delete pointmap_;
        pointmap_ = new QPointMapMesh(scene_);
        pointmap_->setObjectName("pointmap");
    }
    pointmap_->loadPLY(filename, w, h, false, 0.0);
//    pointmap_->loadPLY(filename, w, h, scene_->getProjectionMatrix(), scene_->getDefaultModelMatrix());
    need_update_buffer_ = true;
    clearResults();
    updateBuffer();
}


void QModelFinder::loadModel(const QString &filename)
{
    mesh_->loadFile(filename, true);
}

Q3DScene *QModelFinder::getScene()
{
    return scene_;
}

QMesh3D *QModelFinder::getMesh()
{
    return mesh_;
}

QMaterial *QModelFinder::getMaterial()
{
    return material_;
}

void QModelFinder::updateBuffer()
{
    if(!pointmap_ || !z_buffer_ || !normal_buffer_)
    {
        return;
    }
    QMatrix4x4 projection_matrix = getScene()->getProjectionMatrix();
    QMatrix4x4 model_matrix = getScene()->getModelMatrix();
    QMatrix4x4 default_matrix = getScene()->getDefaultModelMatrix();

    clock_t t0 = clock();

    glViewport(0, 0, z_buffer_->width(), z_buffer_->height());

    pointmap_->getMaterial()->addUniformValue("pack_range", float(pack_range_));
    pointmap_->getMaterial()->addUniformValue("u_pack_minval", float(scene_->getPackMinval()));
    pointmap_->getMaterial()->addUniformValue("u_pack_scale", float(scene_->getPackScale()));

    pointmap_->renderMap(z_buffer_, QPointMapMesh::Z_Map, projection_matrix, default_matrix);
    pointmap_->getMaterial()->addUniformValue("u_max_depth", u_max_depth_);
    pointmap_->getMaterial()->addUniformValue("u_min_depth", u_min_depth_);

    z_map_ = z_buffer_->toImage(true);


    pointmap_->renderMap(normal_buffer_, QPointMapMesh::Normal_Map, projection_matrix, default_matrix);

    nor_map_ = normal_buffer_->toImage(true);
    mesh_->getMaterial()->addUniformValue("pack_range", float(pack_range_));
    mesh_->getMaterial()->addUniformTexture("z_map", z_buffer_->texture());
    mesh_->getMaterial()->addUniformTexture("nor_map", normal_buffer_->texture());
    mesh_->getMaterial()->addUniformValue("u_state", int(1));
    mesh_->getMaterial()->addUniformValue("u_mscore", float(u_mscore_));
    mesh_->getMaterial()->addUniformValue("u_trunc", float(u_trunc_));
    mesh_->getMaterial()->addUniformValue("u_pack_minval", float(scene_->getPackMinval()));
    mesh_->getMaterial()->addUniformValue("u_pack_scale", float(scene_->getPackScale()));
    mesh_->getMaterial()->addUniformValue("u_min_depth", u_min_depth_);

//    mesh_->getMaterial()->addUniformValue("u_min_depth",float(u_min_depth_));
    qDebug() << (clock() - t0) / (CLOCKS_PER_SEC / 1000) << "ms: update buffer";

}

void QModelFinder::findModels(const QList<QVector3D> &sample_points)
{

    int group_size = std::min(config_->getMaxOnceQueryPointsNumber(), sample_points.size());
    qDebug() << "group_size: " << group_size ;
    seeds_.clear();
    for(int i = 0; i < group_size; i++)
    {
        this->appendSeedsAtSamplePoint(sample_points[i], seeds_, visited_points_.size());
        visited_points_.push_back(sample_points[i]);
    }

    int iter = config_->getOutterIterationNum();
    int o_result_size = results_.size();
    float score_level = u_score_threshold_;
    u_score_threshold_ = 0.0;
    u_feature_score_threshold_ = -1;
    while(iter--)
    {
        if(iter == 0)
        {
            u_score_threshold_ = getScoreThreshold();
            u_feature_score_threshold_ = getFeatureScoreThreshold();
        }
        findAtSeeds();
        seeds_.clear();

        if(iter == 0)
        {
            for(int i = o_result_size; i < results_.size(); i++)
            {
                qDebug() << i << ": " << results_[i].score_;
            }
            break;
        }
        for(int i = o_result_size; i < results_.size(); i++)
        {
            ResultTransform rt = results_[i];
            checkResult(rt);
            SeedPoint seed;
            seed.score_ = 0.0;
            seed.s_pos_ = rt.s_pos_;
            seed.offset_ = rt.offset_;
            seed.o_pos_ = seed.offset_;
            seed.seed_id_ = rt.seed_id_;
            seed.eular_ = rt.euler_;
            appendSeedsBySeed(seed, seeds_);
        }
        while(results_.size() > o_result_size) results_.pop_back();
    }
    u_score_threshold_ = score_level;

}

int QModelFinder::findModels()
{
    if(need_update_buffer_)
    {
        this->updateBuffer();
        need_update_buffer_ = false;
    }
    getScene()->resize(z_buffer_->width(), z_buffer_->height());
    QList<QVector3D> sample_points;
    getSamplePoints(sample_points);

    for(int i = 0; i < sample_points.size(); i++)
    {
        sample_points[i].setZ(rand());
    }
    std::sort(sample_points.begin(), sample_points.end(), [&](const QVector3D & v0, const QVector3D & v1)
    {
        return (v0.z() > v1.z());
    });
    for(int i = 0; i < sample_points.size(); i++)
    {
        SeedPoint seed = getSeedBySamplePoint( sample_points[i]);
        sample_points[i].setZ(seed.o_pos_.z());
    }

    for(int i = 0; i < sample_points.size();)
    {
        bool flag = false;
        for(const QVector3D& p : visited_points_)
        {
            QVector3D df = p - sample_points[i];
            df[2] = 0;
            if(df.length()  < 50)
            {
                flag = true;
                break;
            }
        }

        if(flag)
        {
            std::swap(sample_points[i], sample_points[sample_points.length() - 1]);
            sample_points.pop_back();
        }
        else
        {
            i++;
        }
    }





    clock_t t0 = clock();


    findModels(sample_points);
//    tfbo_buffer_->toImage(false).save("tt.png");

    qDebug() << (clock() - t0) / (CLOCKS_PER_SEC / 1000) << "ms: find models";
    qDebug() << sample_points.size() << " " << results_.size() << " ";

    return sample_points.size();
}

void QModelFinder::quickCheckAtResults(QList<QModelFinder::ResultTransform> &results, double score_threshold, int inner_iternum)
{
    if(config_->getTargetSize() == 0) return;

    clock_t t0 = clock();
    seeds_.clear();
    u_local_iteration_num_ = inner_iternum;
    u_score_threshold_ = score_threshold;
    for(int i = 0; i < results.size() && i < 64; i++)
    {
        ResultTransform rt = results[i];
        SeedPoint seed;

        QVector3D s_pos = (scene_->getProjectionMatrix() * scene_->getDefaultModelMatrix()).map(rt.offset_);

        s_pos = s_pos + QVector3D(1, 1, 0);
        s_pos /= 2.0;
        s_pos.setZ((seed.offset_ + u_initial_offset_)[2]);

//        qDebug() << s_pos << " " << rt.s_pos_;
        seed.score_ = 0.0;
        seed.s_pos_ = QVector2D( s_pos.x(), s_pos.y());
        seed.offset_ = rt.offset_;
        seed.o_pos_ = rt.offset_;
        seed.seed_id_ = i;
        seed.eular_ = rt.euler_;
        seed.target_id_ = rt.target_id_;
        seed.mesh_ = mesh_;// model_dict_[config_->getTargetInfo(0)->modelname_];

//        visited_points_.push_back(rt.s_pos_);
        seeds_.append(seed);
    }
    findAtSeeds();
    results = results_;
    seeds_.clear();
    u_local_iteration_num_ = config_->getInnerIterationNum();
    u_score_threshold_ = config_->getScoreThreshold();

    for(auto &rt : results)
    {
        qDebug() << "result: " << rt.seed_id_ << " " << rt.score_;
    }
    qDebug() << (clock() - t0) / (CLOCKS_PER_SEC / 1000) << "ms: quick find models";

}

void QModelFinder::quickFindAllModels()
{
    auto tmp_results = results_;

    clearResults(true);
//    loadPointMapPLY("12.ply", -1);

    quickCheckAtResults(tmp_results, -1);


//    clock_t t0 = clock();
//    if(need_update_buffer_)
//    {
//        this->updateBuffer();
//        need_update_buffer_ = false;
//    }

//    results_.clear();

//    QList<QVector3D> sample_points;
//    results_.clear();
//    getSamplePoints(sample_points);
//    seeds_.clear();

//    for(int i = 0; i < sample_points.size(); i++)
//    {
//        if(i % config_->getMaxOnceQueryPointsNumber() == 0)
//        {
//            if(!seeds_.empty())
//            {
//                findAtSeeds();
//            }
//            seeds_.clear();
//        }
//        QVector3D sp = sample_points[i];
//        this->appendSeedsAtSamplePoint(sp, seeds_, i);
////        qDebug()<<seeds_.size();
//    }
//    if(!seeds_.empty())
//    {
//        findAtSeeds();
//    }
//    seeds_.clear();
//    qDebug() << (clock() - t0) / (CLOCKS_PER_SEC / 1000) << "ms: find models";
//    qDebug() << sample_points.size() << " " << results_.size() << " ";
//    glFlush();
}
void QModelFinder::findAtSeeds()
{
    int tfbo_size = config_->getCanvasSize();
    int ssize = config_->getPatchNumber();
    QMatrix4x4 projection_matrix = getScene()->getProjectionMatrix();
    QMatrix4x4 default_matrix = getScene()->getDefaultModelMatrix();
    clock_t t0 = clock();
    mesh_->getMaterial()->addUniformValue("u_state", int(1));
    qreal aspect = z_buffer_->width() / (qreal)(z_buffer_->height());
    int tw = tfbo_size / ssize;
    int th = tfbo_size / ssize;

    int iter = u_local_iteration_num_;

    int bk_level = config_->getBkCost();
    QVector4D bkcolor = QMaterial::packInt(bk_level);


    while(iter--)
    {
        tfbo_buffer_->bind();
        glViewport(0, 0, tfbo_buffer_->width(), tfbo_buffer_->height());
        glClearColor(bkcolor[0], bkcolor[1], bkcolor[2], bkcolor[3]);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_TEXTURE);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glDisable(GL_BLEND);
        QList<QVector3D> t_offset_list;
        QList<QVector3D> t_eular_list;
        for(int i = 0; i < ssize * ssize; i++)
        {
            int cx = tw * (i % ssize) + tw * 0.5;
            int cy = th * (i / ssize) + th * 0.5;

            int si = i /  QFinderConfig::DtSize;
            int sj = i %  QFinderConfig::DtSize;
            if(si >= seeds_.size()) continue;
            SeedPoint seed = seeds_[si];
            QMesh3D *mesh = seed.mesh_;
            auto *target = config_->getTargetInfo(seed.target_id_);
            if(!target)
            {
                qDebug() << "Error: target not found";
                return;
            }
            if(!mesh)
            {
                mesh = mesh_;
            }
            mesh->getMaterial()->addUniformValue("u_state", int(1));
            int dtx = tw * target->scissor_level_ * seed.s_pos_.x();
            int dty = tw / aspect * target->scissor_level_ * seed.s_pos_.y();

            glViewport(cx - dtx, cy - dty, tw * target->scissor_level_, tw / aspect * target->scissor_level_);

            glEnable(GL_SCISSOR_TEST);
            glScissor(cx - tw / 2, cy - tw / 2, tw, tw);

            QFinderConfig::RigidTransform dv = target->dt_[sj];

            mesh->setEuler(seed.eular_ + dv.euler_ * target->angle_scale_);
//            mesh->setEuler(seed.eular_);
//            if(dv.euler_.length() > 1e-6)
//            {
//                mesh->applyLeftEuler(dv.euler_ * target->angle_scale_);
//            }
            mesh->setOffset(seed.offset_ + dv.offset_ * target->offset_scale_);

            t_offset_list.push_back(mesh->getOffset());
            t_eular_list.push_back(mesh->getEuler());
            mesh->render(projection_matrix, default_matrix);


            glDisable(GL_SCISSOR_TEST);
        }
        glClearColor(0, 0, 0, 1);

        tfbo_buffer_->bindDefault();
        glViewport(0, 0, z_buffer_->width(), z_buffer_->height());

        evaluator_->setToImage(false);
        evaluator_->getSumTexture(tfbo_buffer_->texture(), tfbo_buffer_->width(), tfbo_buffer_->height(), QShaderEvaluator::Size_4096);
        QImage res = evaluator_->getFrameBufferObj(QShaderEvaluator::MapSize(ssize))->toImage(false);
        double mval = 0.0;

        for(int j = 0; j < res.height(); j++)
        {
            for(int i = 0; i < res.width(); i++)
            {
                QRgb rgb = res.pixel(i, j);

                QVector4D vv(qRed(rgb), qGreen(rgb), qBlue(rgb), qAlpha(rgb));
                vv /= 255.0;
                double val = (QMaterial::unpackInt(vv));
                int index = j * res.width() + i;
                int si = index / QFinderConfig::DtSize;
                if(val > mval)
                {
                    mval = val;
                }
                if(si >= seeds_.size()) continue;
                auto *target = config_->getTargetInfo( seeds_[si].target_id_);
                val = (val  - (tw * tw * bk_level))
                      / (tw * tw / aspect * target->scissor_level_ * target->scissor_level_);
                if(seeds_[si].score_ < val)
                {

                    seeds_[si].score_ = val;
                    seeds_[si].offset_ = t_offset_list[index];
                    seeds_[si].eular_ = t_eular_list[index];
                }
            }

        }


    }
    result_dict_.clear();
    for(int i = 0; i < seeds_.size(); i++)
    {
        int si = seeds_[i].seed_id_;
        qreal score = (seeds_[i].score_);
        if(score < 0) score = 0;
        for(int j = 0;  j < 3; j++)
        {
            seeds_[i].eular_[j]  = (seeds_[i].eular_[j] - 360 * int(seeds_[i].eular_[j] / 360));
        }
        if(!result_dict_.count(si))
        {
            ResultTransform rt;
            rt.seed_id_ = si;
            rt.score_ = score;
            rt.offset_ = seeds_[i].offset_;
            rt.euler_ = seeds_[i].eular_;
            rt.s_pos_ = seeds_[i].s_pos_;
            rt.name_ = seeds_[i].mesh_->objectName();
            rt.target_id_ = seeds_[i].target_id_;

            result_dict_[si] = rt;
        }
        else
        {
            ResultTransform &rt = result_dict_[si];
            if(score > rt.score_ || (score > rt.score_ - 1e-7 && seeds_[i].offset_[2] > rt.offset_[2]))
            {
                rt.score_ = score;
                rt.seed_id_ = si;
                rt.offset_ = seeds_[i].offset_;
                rt.euler_ = seeds_[i].eular_;
                rt.s_pos_ = seeds_[i].s_pos_;
                rt.name_ = seeds_[i].mesh_->objectName();
                rt.target_id_ = seeds_[i].target_id_;
                result_dict_[si] = rt;
            }
        }

    }

    QList<int> ids = result_dict_.keys();
    result_dict_.size();
    for(int id : ids)
    {
        ResultTransform rt = result_dict_[id];
        if(rt.score_ >= u_score_threshold_ - 1e-7)
        {
            if(rt.target_id_ < 0)
            {
                continue;
            }
            auto *target = config_->getTargetInfo(rt.target_id_);
            // 这里的mesh_和axes_只是用来辅助矩阵计算
            mesh_->setEuler(rt.euler_);
            mesh_->setOffset(rt.offset_);
            if(!target->featurepoints_.empty())
            {
                float tcount = 0;
                for(auto feature : target->featurepoints_)
                {
                    axes_->setEuler(feature.euler_);
                    axes_->applyLeftEuler(rt.euler_);
                    QVector3D pos = rt.offset_ + mesh_->localRotate( feature.offset_);
                    QVector3D nor = axes_->localRotate(QVector3D(0, 0, 1));

                    PointFittingScore posfit = getPointFittingScoreAt(pos, nor);
                    if(posfit.s_pos_[2] > config_->getMinDepth())
                    {
                        if((posfit.dist_ > config_->getDistTrunc() * 2.5) || posfit.cosval_ < 0.88)
                        {
//                            qDebug() << posfit.dist_ << " " << posfit.cosval_;
                            tcount += 1.0;
                        }
                    }
                    else
                    {
                        tcount += 0.15;
                    }


                }

//                qDebug() << 1.0 - tcount / target->featurepoints_.size();
                rt.score_ =  (1.0 - tcount / target->featurepoints_.size()) * rt.score_;
                if(rt.score_ < u_score_threshold_ - 1e-7) continue;
                if(u_feature_score_threshold_ > 0)
                {
                    rt.score_ = (1.0 - tcount / target->featurepoints_.size());

                    if(rt.score_ < u_feature_score_threshold_ - 1e-7) continue;
                }
            }
//            if(rt.score_ < u_score_threshold_ - 1e-7) continue;


            QVector3D maxUp(-1, -1, -999);
            QMatrix4x4 matrix = scene_->getDefaultModelMatrix();
            QVector3D moffset = matrix.map(QVector3D(0, 0, 0));
            for(int i = 0; i < target->pickpoints_.size(); i++)
            {
                auto rigid = target->pickpoints_[i];
                axes_->setEuler(rigid.euler_);
                axes_->applyLeftEuler(rt.euler_);
                QVector3D tUp = matrix.map( axes_->localRotate(QVector3D(0, 0, 1))) - moffset;


//                qDebug() << tUp;

                if(tUp.z() > maxUp.z())
                {
                    maxUp = tUp;
                    rt.pick_euler_ = axes_->getEuler();
                    rt.pick_offset_ = mesh_->localRotate(rigid.offset_);
                    rt.place_euler_ = target->placepoints_[i].euler_;
                    rt.place_offset_ = target->placepoints_[i].offset_;
                }

            }
            results_.push_back(rt);
        }
    }
    glClearColor(0, 0, 0, 1);
    if(results_.size() > 0)
    {
        if(model_dict_.count(results_.back().name_))
        {
            mesh_ = model_dict_[results_.back().name_];
        }
        mesh_->setOffset(results_.back().offset_);
        mesh_->setEuler(results_.back().euler_);

    }
//    tfbo_buffer_->toImage(false).save("tt.png");
}

qreal QModelFinder::localSearching(QVector2D sp)
{

    seeds_.clear();
    SeedPoint seed;
    seed.s_pos_ = sp;
    seed.o_pos_ = mesh_->getOffset();
    seed.offset_ = seed.o_pos_;
    seed.eular_ = mesh_->getEuler();
    seed.mesh_ = mesh_;
    appendSeedsBySeed(seed, seeds_, 0);
    int o_iter_num =  u_local_iteration_num_;
    float o_score_level = u_score_threshold_;
    u_local_iteration_num_ = 1;
    u_score_threshold_ = -10.0;
    u_feature_score_threshold_ = 0.1;

    int result_size = results_.size();
//    findAtSeeds();

    float score = 0.0;
//    while(results_.size() > result_size)
//    {
//        ResultTransform rt = results_.back();
//        checkResult(rt);
//        if(rt.score_ > score)
//        {
//            if(model_dict_.count(rt.name_))
//            {
//                mesh_ = model_dict_[rt.name_];
//            }
//            mesh_->setEuler(rt.euler_);
//            mesh_->setOffset(rt.offset_);
//            score = rt.score_;
//        }
//        results_.pop_back();
//    }
    if(config_->getTargetSize() > 0)
    {
        auto *target = config_->getTargetInfo(0);
        if(!target->featurepoints_.empty())
        {
            clock_t t0 = clock();
            int iter = 10;
            while(iter--)
            {
                typedef QICPAlign::PtPair PtPair;
                std::vector<PtPair> pairs;

                QMatrix4x4 y_matrix;
                QVector3D euler = mesh_->getEuler();

                QVector3D offset = mesh_->getOffset();
                y_matrix.rotate(euler[0], QVector3D(1, 0, 0));
                y_matrix.rotate(euler[1], QVector3D(0, 1, 0));
                y_matrix.rotate(euler[2], QVector3D(0, 0, 1));
                y_matrix.translate(mesh_->getOffset());
                float tcount = 0;
                float dist_sum = 0.0;
                for(auto feature : target->featurepoints_)
                {
                    axes_->setEuler(feature.euler_);
                    axes_->applyLeftEuler(euler);
                    QVector3D pos = offset + mesh_->localRotate( feature.offset_);
                    QVector3D nor = axes_->localRotate(QVector3D(0, 0, 1));
                    PointFittingScore posfit = getPointFittingScoreAt(pos, nor);


                    if(posfit.s_pos_[2] > config_->getMinDepth())
                    {
                        if((posfit.dist_ > config_->getDistTrunc() * 2.5) || posfit.cosval_ < 0.88)
                        {
//                            qDebug() << posfit.dist_ << " " << posfit.cosval_;
                            tcount += 1.0;
                            dist_sum += posfit.dist_;
                        }
                    }
                    else
                    {
                        tcount += 0.15;
                        dist_sum += config_->getDistTrunc();
                    }

                    if(posfit.s_pos_[2] <= config_->getMinDepth() + 1.0 ) continue;
                    PtPair pair(QMathUtil::toVec( posfit.s_pos_),
                                QMathUtil::toVec( posfit.s_nor_),
                                QMathUtil::toVec(  posfit.pos_),
                                QMathUtil::toVec( posfit.nor_));
                    pairs.push_back(pair);
                }
                qDebug()<<"score - :"<<" "<<(1-tcount/ target->featurepoints_.size())<<" "<<dist_sum<<" "<<iter;
                if(iter==0) break;
                Vec3 centroid1;
                Vec3 centroid2;

                for(const PtPair &pair : pairs)
                {
                    centroid1 += pair.p1;
                    centroid2 += pair.p2;
                }
                centroid1 /= pairs.size();
                centroid2 /= pairs.size();
                QMatrix4x4 x_matrix;
                QICPAlign::align_symm(pairs,centroid1,centroid2,x_matrix);

                y_matrix = x_matrix * y_matrix;

                QVector3D euler1 = QMathUtil::fromMatrixToEuler(y_matrix);
                QVector4D tmp_offset = x_matrix.map(QVector4D(0,0,0,1));
                QVector3D offset1(tmp_offset.x(),tmp_offset.y(),tmp_offset.z());
                mesh_->setEuler(euler1);
                mesh_->setOffset(offset + QVector3D(0,0,centroid1[2]-centroid2[2]));
            }

            qDebug()<<clock() - t0<<"ms";
        }
    }

    u_local_iteration_num_ = o_iter_num;
    u_score_threshold_ = o_score_level;
    u_feature_score_threshold_ = getFeatureScoreThreshold();
    return score;
}

void QModelFinder::setViewByPointsCenter()
{
    QVector3D vmax =  pointmap_->get_bbox_vmax();
    QVector3D vmin =  pointmap_->get_bbox_vmin();

    QVector3D vcenter = (vmax + vmin) * 0.5;

    qDebug() << vmax << " " << vmin << " " << vcenter;
    vcenter.setZ(vmax.z());
    QMatrix4x4 matrix;

    matrix.translate(-vcenter - QVector3D(0, 0, 500));

    scene_->setDefaultModelMatrix(matrix);

    scene_->setDefaultView();

}

void QModelFinder::appendSeedsAtSamplePoint(const QVector3D &sample_point, QList<QModelFinder::SeedPoint> &seeds, int id)
{
    SeedPoint sp = getSeedBySamplePoint(sample_point);

    for(int i = 0; i < config_->getTargetSize(); i++)
    {
        QFinderConfig::TargetInfo *target = config_->getTargetInfo(i);
        sp.eular_ = target->init_euler_;
        sp.seed_id_ = id;
        sp.mesh_ = model_dict_[target->modelname_];
        sp.target_id_ = i;
        seeds.append(sp);
    }

}

void QModelFinder::appendSeedsBySeed(const SeedPoint &seed, QList<QModelFinder::SeedPoint> &seeds, int id)
{
    for(int i = 0; i < config_->getTargetSize(); i++)
    {
        QFinderConfig::TargetInfo *target = config_->getTargetInfo(i);
        SeedPoint sp = seed;
        sp.eular_ = target->refine_euler_ + sp.eular_; // QMathUtil::mulEuler(target->refine_euler_,sp.eular_);
        sp.seed_id_ = seed.seed_id_;
        sp.mesh_ = model_dict_[target->modelname_];
        sp.target_id_ = i;
        seeds.append(sp);
    }
}

void QModelFinder::checkResult(QModelFinder::ResultTransform &rt)
{

    QVector3D pos = rt.offset_ + rt.pick_offset_;
    axes_->setEuler(rt.pick_euler_);
    QVector3D nor = axes_->localRotate(QVector3D(0, 0, 1));

    QVector3D spos = (scene_->getProjectionMatrix() * scene_->getDefaultModelMatrix()) * pos;
    spos /= spos[2];

    spos = (spos + QVector3D(1, 1, 1)) * 0.5;


    SeedPoint tseed = getSeedBySamplePoint(QVector3D(z_map_.width() * spos.x(), z_map_.height() * (1.0 - spos.y()), 1.0));
    QVector3D z_pos = tseed.offset_ - u_initial_offset_;

    QRgb nor_rgb = nor_map_.pixel(nor_map_.width() * spos.x(), z_map_.height() * (1.0 - spos.y()));

    QVector3D nor_cv(qRed(nor_rgb), qGreen(nor_rgb), qBlue(nor_rgb));
    nor_cv /= 255.0;

    nor_cv = (nor_cv * 2.0 - QVector3D(1, 1, 1));

    nor_cv.normalize();

    if((fabs(z_pos[2] - pos[2]) > 10.0) || QVector3D::dotProduct(nor, nor_cv) < 0.9)
    {
        QVector3D euler0 = QMathUtil::fromVectorTransformToEuler(nor, nor_cv);

        axes_->setEuler(rt.euler_);
        axes_->applyLeftEuler(euler0);

        rt.euler_ = axes_->getEuler();

        if(z_pos[2] > config_->getMinDepth())
        {
//            rt.offset_ = rt.offset_ + QVector3D(0, 0, (z_pos[2] - pos[2]));
        }

    }
}

QModelFinder::PointFittingScore QModelFinder::getPointFittingScoreAt(QVector3D pos, QVector3D nor)
{
    QList<float> dlist;
    dlist.push_back(0);
    dlist.push_back(1);
    dlist.push_back(-1);
    dlist.push_back(2);
    dlist.push_back(-2);
    QVector3D t_pos;
    QVector3D t_nor;
    float mdist = -1;
    for(float dt : dlist)
    {
        QVector3D screen_pos = (scene_->getProjectionMatrix() * scene_->getDefaultModelMatrix()) * (pos+dt*nor);
        screen_pos /= screen_pos[2];
        screen_pos = (screen_pos + QVector3D(1, 1, 1)) * 0.5;

        if(screen_pos.x() < 0 || screen_pos.x() >= 1.0 || screen_pos.y() < 0 || screen_pos.y() >= 1.0)
        {
            continue;
        }
        QVector2D sp(z_map_.width() * screen_pos.x(), z_map_.height() * (1.0 - screen_pos.y()));
        QRgb rgb = z_map_.pixel(sp.x(), sp.y());
        QVector4D vv(qRed(rgb), qGreen(rgb), qBlue(rgb), qAlpha(rgb));
        vv /= 255;
        qreal zval = QMaterial::unpackInt(vv) / scene_->getPackScale() + scene_->getPackMinval();
        QVector3D raypoint, raydir;
        scene_->getScreenRay(sp.x(), z_map_.height() - sp.y(), raypoint, raydir);
        qreal t0 = QVector3D::dotProduct(raydir, QVector3D(0, 0, 1));
        qreal t1 =  QVector3D::dotProduct((raypoint - QVector3D(0, 0, zval)), QVector3D(0, 0, 1));
        qreal k = -t1 / t0;
        QVector3D z_pos = raypoint + k * raydir;


        QRgb nor_rgb = nor_map_.pixel(nor_map_.width() * screen_pos.x(), nor_map_.height() * (1.0 - screen_pos.y()));
        QVector3D nor_cv(qRed(nor_rgb), qGreen(nor_rgb), qBlue(nor_rgb));
        nor_cv /= 255.0;
        nor_cv = (nor_cv * 2.0 - QVector3D(1, 1, 1));
        nor_cv.normalize();
        float dist = (pos - z_pos).length();
        if(mdist < 0 ||  dist < mdist)
        {
            mdist = dist;
            t_pos = z_pos;
            t_nor = nor_cv;
        }


    }





    PointFittingScore posfit;
    posfit.pos_ = pos;
    posfit.nor_ = nor;
    posfit.s_pos_ = t_pos;
    posfit.s_nor_ = t_nor;


    posfit.dist_ = mdist;
    posfit.cosval_ = QVector3D::dotProduct(posfit.nor_, posfit.s_nor_);

    return posfit;
}

void QModelFinder::setSamplePoints(QList<QVector3D> &sample_points)
{
    u_sample_points_ = sample_points;
}


void QModelFinder::getSamplePoints(QList<QVector3D> &sample_points)
{
    if(u_sample_points_.size() > 0)
    {
        sample_points = u_sample_points_;
        return;
    }
    if(!z_buffer_) return;
    QList<SampleArea> area_list;
    QOpenGLFramebufferObject  b_fbo(z_buffer_->width(), z_buffer_->height(), QOpenGLFramebufferObject::CombinedDepthStencil);
    b_fbo.bind();
    glViewport(0, 0, b_fbo.width(), b_fbo.height());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_TEXTURE);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    pointmap_->getMaterial()->addUniformValue("u_state", int(4));
    pointmap_->render(scene_->getProjectionMatrix(), scene_->getModelMatrix());
    b_fbo.bindDefault();
    QImage image = b_fbo.toImage(true);
    for(int i = 0; i < image.width(); i++)
    {
        for(int j = 0; j < image.height(); j++)
        {
            QRgb rgb = image.pixel(i, j);
            if(qRed(rgb) == 0 && qBlue(rgb) == 0)
            {
                image.setPixel(i, j, qRgb(255, 0, 0));
            }
        }
    }
    QList<QVector2D> boundary_list;
    int dx[8] = {1, -1, 0, 0, 1, 1, -1, -1};
    int dy[8] = {0, 0, 1, -1, 1, -1, 1, -1};
    for(int i = 1; i < image.width() - 1; i++)
    {
        for(int j = 1; j < image.height() - 1; j++)
        {
            QRgb rgb = image.pixel(i, j);
            if(rgb == qRgb(255, 255, 0)) continue;
            if(rgb == qRgb(255, 0, 0)) continue;
            for(int k = 0; k < 8; k++)
            {
                int px = i + dx[k];
                int py = j + dy[k];
                if(image.pixel(px, py) == qRgb(255, 0, 0))
                {

                    image.setPixel(i, j, qRgb(255, 255, 0));
                    boundary_list.push_back(QVector2D(i, j));
                    break;
                }
            }

        }
    }
    QList<QVector2D> tblist;
    for(auto p : boundary_list)
    {
        if(image.pixel(p.x(), p.y()) == qRgb(255, 255, 1))
        {
            continue;
        }
        QList<QVector2D> queue;
        queue.push_back(p);
        image.setPixel(p.x(), p.y(), qRgb(255, 255, 1));
        for(int qi = 0; qi < queue.size(); qi++)
        {
            QVector2D p0 = queue[qi];
            for(int k = 0; k < 8; k++)
            {
                int px = p0.x() + dx[k];
                int py = p0.y() + dy[k];
                if(px < 0 || px >= image.width()) continue;
                if(py < 0 || py >= image.height()) continue;
                if(image.pixel(px, py) == qRgb(255, 255, 0))
                {
                    image.setPixel(px, py, qRgb(255, 255, 1));
                    queue.push_back(QVector2D(px, py));
                }
            }
        }
        if(queue.size() > u_part_least_size_ * z_buffer_->width() / b_fbo.width())
        {
            tblist.append(queue);
        }
        else
        {
            for(auto qp : queue)
            {
                image.setPixel(qp.x(), qp.y(), qRgb(0, 0, 0));
            }
        }
    }
    boundary_list = tblist;
    int target_level = u_boundary_level_;

    for(int level = 0; level < target_level; level++)
    {
        QList<QVector2D> tlist;
        for(auto p : boundary_list)
        {
            image.setPixel(p.x(), p.y(), qRgb(255, 0, 0));
        }

        for(QVector2D p : boundary_list)
        {
            for(int k = 0; k < 8; k++)
            {
                int px = p.x() + dx[k];
                int py = p.y() + dy[k];
                if(px < 0 || px >= image.width()) continue;
                if(py < 0 || py >= image.height()) continue;
                if(image.pixel(px, py) != qRgb(255, 0, 0))
                {
                    image.setPixel(px, py, qRgb(255, 0, 0));
                    tlist.push_back(QVector2D(px, py));
                }
            }
        }
        boundary_list = tlist;
    }

    boundary_list.clear();
    for(int i = 1; i < image.width() - 1; i++)
    {
        for(int j = 1; j < image.height() - 1; j++)
        {
            QRgb rgb = image.pixel(i, j);
            if(rgb == qRgb(255, 255, 0)) continue;
            if(rgb == qRgb(255, 0, 0)) continue;
            for(int k = 0; k < 8; k++)
            {
                int px = i + dx[k];
                int py = j + dy[k];
                if(image.pixel(px, py) == qRgb(255, 0, 0))
                {

                    image.setPixel(i, j, qRgb(255, 255, 0));
                    boundary_list.push_back(QVector2D(i, j));
                    break;
                }
            }

        }
    }
    for(QVector2D p : boundary_list)
    {
        image.setPixel(p.x(), p.y(), qRgb(255, 255, 0));
    }

    for(auto p : boundary_list)
    {
        if(image.pixel(p.x(), p.y()) == qRgb(255, 255, 1))
        {
            continue;
        }
        QList<QVector2D> queue;
        queue.push_back(p);
        image.setPixel(p.x(), p.y(), qRgb(255, 255, 1));
        for(int qi = 0; qi < queue.size(); qi++)
        {
            QVector2D p0 = queue[qi];
            for(int k = 0; k < 8; k++)
            {
                int px = p0.x() + dx[k];
                int py = p0.y() + dy[k];
                if(px < 0 || px >= image.width()) continue;
                if(py < 0 || py >= image.height()) continue;
                if(image.pixel(px, py) == qRgb(255, 255, 0))
                {
                    image.setPixel(px, py, qRgb(255, 255, 1));
                    queue.push_back(QVector2D(px, py));
                }
            }
        }
        if(queue.size() < u_part_least_size_ * z_buffer_->width() / b_fbo.width())
        {
            continue;
        }
        QVector2D center;
        for(auto qp : queue)
        {
            center += qp;
        }
        center /= queue.size();
        SampleArea area;
        area.center_ = center;
        qreal expand_size = config_->getSampleAreaExpandLevel() + 1;
        for(auto qp : queue)
        {
            area.bbox_ =  area.bbox_.united(QRectF(qp.x() - int(expand_size * 0.5), qp.y() - int(expand_size * 0.5),
                                                   expand_size, expand_size));
        }
        area.pointsize_ = queue.size();
        area_list.push_back(area);

    }

    for(int tt = 0; tt < 3; tt++)
    {
        bool merged_happened = false;
        for(int i = 0; i < area_list.size(); i++)
        {
            if(!area_list[i].is_valid_) continue;
            qreal areaA = area_list[i].bbox_.width() * area_list[i].bbox_.height();
            for(int j = i + 1; j < area_list.size(); j++)
            {
                QRectF intersectionRect = area_list[i].bbox_.intersected(area_list[j].bbox_);
                qreal areaB = area_list[j].bbox_.width() * area_list[j].bbox_.height();

                if(areaA < areaB) areaB = areaA;

                double iou = (intersectionRect.width() * intersectionRect.height())
                             / (areaB + 1e-7);
                if(iou > config_->getSampleAreaIOUThreshold())
                {
                    area_list[j].merge(area_list[i]);
                    area_list[i].is_valid_ = false;
                    merged_happened = true;
                    break;
                }
            }
        }
        if(!merged_happened) break;
    }


    for(const auto &area : area_list)
    {
        if(area.is_valid_)
        {

            sample_points.push_back(QVector3D(area.center_.x() * z_buffer_->width() / b_fbo.width(),
                                              area.center_.y() *z_buffer_->height() / b_fbo.height(), 0));
        }
    }
    if(is_debug_mode_)
    {
        QImage image1 =  b_fbo.toImage(true);
        for(int i = 0; i < image.width(); i++)
        {
            for(int j = 0; j < image.height(); j++)
            {
                QRgb cc = image.pixel(i, j);
                if(cc == qRgb(255, 255, 0) || cc == qRgb(255, 255, 1) || cc == qRgb(125, 125, 255)) continue;
                QRgb c1 = image1.pixel(i, j);

                cc = qRgb(  (qRed(cc) + qRed(c1)) >> 1,  (qGreen(cc) + qGreen(c1)) >> 1, (qBlue(cc) + qBlue(c1)) >> 1);
                image.setPixel(i, j, cc);
            }
        }
        QPainter painter(&image);
        QPen pen;
        pen.setWidth(2);
        pen.setColor(Qt::white);
        painter.setPen(pen);
        for(const auto &area : area_list)
        {
            if(area.is_valid_)
            {

                pen.setColor(Qt::black);
                pen.setWidth(1);
                painter.setPen(pen);
                painter.drawRect(area.bbox_);

                pen.setWidth(2);
                pen.setColor(Qt::green);
                painter.setPen(pen);
                painter.drawRect(area.center_.x() - 1, area.center_.y() - 1, 2, 2);

            }
        }

        image.save("b.png");
    }
    u_sample_points_ = sample_points;
}

void QModelFinder::setDepthRange(float max_depth, float min_depth)
{
    u_max_depth_ = max_depth;
    u_min_depth_ = min_depth;
}

void QModelFinder::setScoreThreshold(float score)
{
    u_score_threshold_ = score;
}

void QModelFinder::setBoundaryExtendLevel(int level)
{
    u_boundary_level_ = level;
}

bool QModelFinder::isRenderResult() const
{
    return is_show_results_;
}

double QModelFinder::getScoreThreshold() const
{
    return config_->getScoreThreshold();
}

double QModelFinder::getFeatureScoreThreshold() const
{
    return config_->getFeatureScoreThreshold();
}

void QModelFinder::render()
{
    if(need_update_buffer_)
    {
        this->updateBuffer();
        need_update_buffer_ = false;
    }

    QMatrix4x4 projection_matrix = getScene()->getProjectionMatrix();
    QMatrix4x4 model_matrix = getScene()->getModelMatrix();
//    QMatrix4x4 default_matrix = getScene()->getDefaultModelMatrix();
    glViewport(0, 0, this->width(), this->height());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_TEXTURE);
    glEnable(GL_DEPTH_TEST);
//    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    pointmap_->getMaterial()->addUniformValue("u_state", int(6));


    pointmap_->render(projection_matrix, model_matrix);

    if(is_show_results_)
    {
        mesh_->getMaterial()->addUniformValue("u_state", int(2));

        QVector3D euler = mesh_->getEuler();
        QVector3D offset = mesh_->getOffset();
        for(auto rt : results_)
        {
            QMesh3D *mesh = model_dict_.value(rt.name_);
            if(!mesh) continue;
            mesh->setOffset(rt.offset_);
            mesh->setEuler(rt.euler_);
            mesh->render(projection_matrix, model_matrix);

            if(axes_)
            {

                axes_->setEuler(rt.pick_euler_);
//                axes_->applyRightEuler(QVector3D(180, 0, 0));
                axes_->setOffset(rt.offset_ - u_initial_offset_.normalized() * 2.5 + rt.pick_offset_);
                axes_->render(projection_matrix, model_matrix);

            }

        }
        mesh_->setEuler(euler);
        mesh_->setOffset(offset);
        mesh_->render(projection_matrix, model_matrix);
    }
    glDisable(GL_BLEND);
}

void QModelFinder::clearResults(bool only_results)
{
    if(!only_results)
    {
        visited_points_.clear();
        results_.clear();
        u_sample_points_.clear();
    }
    else
    {
        results_.clear();
    }
}

void QModelFinder::clearSamplePoints()
{
    u_sample_points_.clear();
    visited_points_.clear();
}

int QModelFinder::getSamplePointSize() const
{
    return u_sample_points_.size();
}

QVector3D QModelFinder::getClickPos(QVector2D sp) const
{
    int x = z_buffer_->width() * sp.x();
    int y = (z_buffer_->height()) * (1.0 - sp.y());
    SeedPoint seed = getSeedBySamplePoint(QVector3D(x, y, 0)) ;
    return seed.offset_ - u_initial_offset_;
}


QList<QModelFinder::ResultTransform> QModelFinder::getResults() const
{
    return results_;
}

QModelFinder::SeedPoint QModelFinder::getSeedBySamplePoint(const QVector3D &sp) const
{

    SeedPoint seed;
    QRgb rgb = z_map_.pixel(sp.x(), sp.y());
    QVector4D vv(qRed(rgb), qGreen(rgb), qBlue(rgb), qAlpha(rgb));
    vv /= 255;
    qreal zval = QMaterial::unpackInt(vv) / scene_->getPackScale() + scene_->getPackMinval();
    QVector3D raypoint, raydir;
    scene_->getScreenRay(sp.x(), z_map_.height() - sp.y(), raypoint, raydir);
    qreal t0 = QVector3D::dotProduct(raydir, QVector3D(0, 0, 1));
    qreal t1 =  QVector3D::dotProduct((raypoint - QVector3D(0, 0, zval)), QVector3D(0, 0, 1));
    qreal k = -t1 / t0;
    QVector3D intersect = raypoint + k * raydir;

    seed.s_pos_ = QVector2D(sp.x() / z_map_.width(), 1.0 - sp.y() / z_map_.height());
    seed.o_pos_ = intersect + u_initial_offset_;
    seed.offset_ = seed.o_pos_;

    return seed;
}

QMatrix4x4 QModelFinder::ResultTransform::getTransform() const
{
    QMatrix4x4 matrix;
    matrix.setToIdentity();
    matrix.translate(offset_);
    matrix.rotate(euler_[0], QVector3D(1, 0, 0));
    matrix.rotate(euler_[1], QVector3D(0, 1, 0));
    matrix.rotate(euler_[2], QVector3D(0, 0, 1));
    return matrix;
}

QQuaternion QModelFinder::ResultTransform::getQuat() const
{
    QMatrix4x4 matrix = getTransform();
    QQuaternion quat;
    quat = quat.fromRotationMatrix(matrix.normalMatrix());
    return quat;

}

QMatrix4x4 QModelFinder::ResultTransform::getPickTransform() const
{
    QMatrix4x4 matrix;
    matrix.setToIdentity();
    matrix.translate(pick_offset_);
    matrix.rotate(pick_euler_[0], QVector3D(1, 0, 0));
    matrix.rotate(pick_euler_[1], QVector3D(0, 1, 0));
    matrix.rotate(pick_euler_[2], QVector3D(0, 0, 1));
    return matrix;
}

QMatrix4x4 QModelFinder::ResultTransform::getPlaceTransform() const
{
    QMatrix4x4 matrix;
    matrix.setToIdentity();
    matrix.translate(place_offset_);
    matrix.rotate(place_euler_[0], QVector3D(1, 0, 0));
    matrix.rotate(place_euler_[1], QVector3D(0, 1, 0));
    matrix.rotate(place_euler_[2], QVector3D(0, 0, 1));
    return matrix;
}

QVector3D QModelFinder::ResultTransform::getPickEuler(const QString &order) const
{
    QMatrix4x4 matrix;
    matrix.rotate(pick_euler_[0], QVector3D(1, 0, 0));
    matrix.rotate(pick_euler_[1], QVector3D(0, 1, 0));
    matrix.rotate(pick_euler_[2], QVector3D(0, 0, 1));

    return QMathUtil::fromMatrixToEuler(matrix, order);
}

QVector3D QModelFinder::ResultTransform::getPlaceEuler(const QString &order) const
{
    QMatrix4x4 matrix;
    matrix.rotate(place_euler_[0], QVector3D(1, 0, 0));
    matrix.rotate(place_euler_[1], QVector3D(0, 1, 0));
    matrix.rotate(place_euler_[2], QVector3D(0, 0, 1));

    return QMathUtil::fromMatrixToEuler(matrix, order);


}

}
