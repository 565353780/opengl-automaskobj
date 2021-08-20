#ifndef QMODELFINDER_H
#define QMODELFINDER_H
#include "../q3d_global.h"
#include <QObject>
#include <QOpenGLFunctions>
#include <QMap>
#include <QVector3D>
#include <QVector2D>
#include <QImage>
class QOpenGLFramebufferObject;
class QOffscreenSurface;
namespace GCL
{
class QMesh3D;
class Q3DScene;
class QPointMapMesh;
class QShaderEvaluator;
class QFinderConfig;
class QMaterial;
class QAxesMesh;
class Q3DGCLSHARED_EXPORT QModelFinder : public QObject, protected QOpenGLFunctions
{
public:
    struct ResultTransform
    {
        QVector3D    euler_;
        QVector3D    offset_;
        QVector2D     s_pos_;
        QString name_{};

        QVector3D pick_euler_;
        QVector3D pick_offset_;

        QVector3D place_offset_;
        QVector3D place_euler_;
        int target_id_{-1};
        int seed_id_{-1};
        QMatrix4x4 getTransform() const;
        QQuaternion getQuat() const;


        QMatrix4x4 getPickTransform() const;
        QMatrix4x4 getPlaceTransform() const;

        QVector3D getPickEuler(const QString &order = "ZYX") const;
        QVector3D getPlaceEuler(const QString &order = "ZYX") const;


        qreal score_{0.0};
    };

    struct SeedPoint
    {
        QVector2D s_pos_;  // screen position (0.0 - 1.0,0.0 - 1.0)
        QVector3D o_pos_;  // origin position
        int seed_id_{-1};
        QVector3D eular_;
        QVector3D offset_;
        qreal score_{0.0};
        QMesh3D *mesh_{nullptr};
        int target_id_{-1};
    };

    struct SampleArea
    {
        void merge(const SampleArea &b)
        {
            center_ = (this->center_ * this->pointsize_) + (b.center_ * b.pointsize_);
            pointsize_ = this->pointsize_ + b.pointsize_;
            center_ /= (pointsize_ + 1e-7);
            bbox_ = bbox_.united(b.bbox_);
        }
        QVector2D center_;
        QRectF bbox_;
        int pointsize_{0};
        bool is_valid_{true};
    };

    struct PointFittingScore
    {
        QVector3D pos_;
        QVector3D nor_;

        QVector3D s_pos_;
        QVector3D s_nor_;

        double dist_{0.0};
        double cosval_{0.0};

    };

    QModelFinder(Q3DScene *scene = nullptr, QObject *parent = nullptr);
    ~QModelFinder();


    void loadConfig(const QString &filename);
    void reloadConfig();
    QFinderConfig *getConfig()
    {
        return config_;
    }

    void create();
    void destroy();

    void makeCurrent();
    void doneCurrent();

    void init();
    void resize(int w, int h);
    void resizeBuffer(int w, int h);
    void render(); // for debug

    int width() const
    {
        return width_;
    }
    int height() const
    {
        return height_;
    }

    void setModelViewMatrix(const QMatrix4x4 &matrix);

    void clearResults(bool only_results = false);

    void clearSamplePoints();

    int getSamplePointSize() const;
    const QImage *getZMap() const
    {
        return &z_map_;
    }

    QVector3D getClickPos(QVector2D sp) const;
public:
    void setPointMap(int w, int h, const std::vector<float> &points, const std::vector<uchar> &colors);
    void loadPointMapPLY(const QString &filename, int w = -1, int h = -1, bool zflip = false, float flip_z = 0.0);
    void loadModel(const QString &filename);

    Q3DScene *getScene();
    QMesh3D *getMesh();
    QMaterial *getMaterial();
public:
    void updateBuffer();

    /**
     * @brief findModels
     * @param sample_points
     * 一般取前八个采样点作为种子点, 并为每个种子点迭代三轮;
     *
     */
    void findModels(const QList<QVector3D> &sample_points);

    int findModels();

    void quickCheckAtResults(QList<ResultTransform> &results,  double score_threshold = 0, int inner_iternum = 5);

    void quickFindAllModels();

    qreal localSearching(QVector2D spos);


    void setViewByPointsCenter();
protected:

    void findAtSeeds();
    void appendSeedsAtSamplePoint(const QVector3D &sample_point, QList<SeedPoint> &seeds, int id = -1);
    void appendSeedsBySeed(const SeedPoint &seed, QList<SeedPoint> &seeds, int id = -1);


    void checkResult(ResultTransform &rt);


    PointFittingScore getPointFittingScoreAt(QVector3D pos, QVector3D nor = QVector3D());
public:
    void setSamplePoints( QList<QVector3D> &sample_points);

    void getSamplePoints( QList<QVector3D> &sample_points);

    void setDistanceTrunc(float trunc);

    void setDepthRange(float max_depth = 1000.0, float min_depth = 0.0);

    void setScoreThreshold(float score);

    void setPartLeastSize(int size)
    {
        u_part_least_size_ = size;
    }

    void setInitOffset(QVector3D o)
    {
        u_initial_offset_ = o;
    }
    QVector3D getInitOffset() const
    {
        return u_initial_offset_;
    }

    void setLocalIterationNum(int num)
    {
        u_local_iteration_num_ = num;
    }
    /**
     * @brief setExtendLevel
     * @param level
     * 边界延拓的级别
     */
    void setBoundaryExtendLevel(int level);


    void setDebugMode(bool t)
    {
        is_debug_mode_ = t;
    }

    void setIsRenderResult(bool t)
    {
        is_show_results_ = t;
    }

    bool isRenderResult() const;

    double getScoreThreshold() const;
    double getFeatureScoreThreshold() const;
public:
    QList<ResultTransform> getResults() const;

private:
    SeedPoint getSeedBySamplePoint(const QVector3D &sp) const;



protected:
    float pack_range_{2048};
    float u_max_depth_{1000.0};
    float u_min_depth_{0.0};
    float u_mscore_{400.0};
    int u_part_least_size_{30};

    QVector3D u_initial_offset_{0, 0, 18};
    QList<QVector3D> u_sample_points_;

    bool is_debug_mode_{false};
    float u_score_threshold_{1.0f};
    float u_feature_score_threshold_{-1.f};
    float u_trunc_{5};
    int u_boundary_level_{6};
    int u_local_iteration_num_{10};
    int width_{640};
    int height_{512};

    int u_group_size_{8};
    int u_scissor_level_{4};
protected:
    Q3DScene *scene_{nullptr};
    QPointMapMesh *pointmap_{nullptr};
    QMesh3D *mesh_{nullptr};

    QShaderEvaluator *evaluator_{nullptr};

    QList<ResultTransform> results_;
    QList<SeedPoint> seeds_;
    QList<QVector3D> visited_points_;

protected:
    QOpenGLFramebufferObject *z_buffer_{nullptr};
    QImage z_map_;
    QImage nor_map_;
    QOpenGLFramebufferObject *normal_buffer_{nullptr};
    QOpenGLFramebufferObject *tfbo_buffer_{nullptr};

    bool need_update_buffer_{true};


protected:
    QOpenGLContext *m_context_{nullptr};
    QOffscreenSurface *m_surface_{nullptr};

    QFinderConfig *config_{nullptr};
    QMaterial *material_{nullptr};
    QMap<QString, QMesh3D *>  model_dict_;

    QMap<int, ResultTransform> result_dict_;

    QString cfg_filename_;

    QAxesMesh *axes_{nullptr};


    bool is_show_results_{true};
};
}
#endif // QMODELFINDER_H
