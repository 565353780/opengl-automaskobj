#ifndef QFINDERCONFIG_H
#define QFINDERCONFIG_H
#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QMap>
#include <QList>
#include "../q3d_global.h"
#include <QVector3D>
#include <QVector4D>
#include <QMatrix4x4>
#include <QFile>
namespace GCL
{
class QMesh3D;
class Q3DGCLSHARED_EXPORT QFinderConfig : public QObject
{
public:
    struct Q3DGCLSHARED_EXPORT RigidTransform
    {
        bool fromJson(const QJsonArray &arr);
        void toJson(QJsonArray &arr) const;

        QMatrix4x4 toMatrix() const;
        QVector4D toDt4() const;
        QVector3D offset_;
        QVector3D euler_;
    };

    struct Q3DGCLSHARED_EXPORT TargetInfo
    {
        void fromJson(const QJsonObject &obj);
        void toJson(QJsonObject &obj) const;

        int scissor_level_{1};
        qreal score_threhold_{2.0};
        qreal score_weight_{1.0};

        qreal angle_scale_{1.0};
        qreal offset_scale_{1.0};
        QString modelname_;
        QString filepath_;
        int id_{-1};
        QList<RigidTransform> pickpoints_;
        QList<RigidTransform> placepoints_;

        QList<RigidTransform> featurepoints_;
        QList<RigidTransform> dt_;
        QVector3D init_euler_;
        QVector3D refine_euler_;
    };
    QFinderConfig(QObject *parent = nullptr);


    void loadFile(const QString &filename);
    void saveFile(const QString &filename);


    void fromJson(const QJsonObject &object);
    void toJson(QJsonObject &object);


public:
    QList<QString> modelnames() const;
    QString  getFilepath(const QString &name) const;
    int getGraidentSize() const;
    int getMaxOnceQueryPointsNumber() const; // 一次最多询问多少个初始查询点
    int getTargetSize() const;
    TargetInfo *getTargetInfo(int i);

    QString getProjectName() const;
    int getCanvasSize() const;
    int getBufferWidth() const
    {
        return buffer_width_;
    }
    int getBufferHeight() const
    {
        return buffer_height_;
    }

    int getPolymapWidth() const
    {
        return polymap_width_;
    }
    int getPolymapHeight() const
    {
        return polymap_height_;
    }
    int getPatchNumber() const;
    int getBkCost() const;
    int getBoundaryExtendLevel() const
    {
        return boundary_extendlevel_;
    }
    int getPartLeastSize() const
    {
        return part_leastsize_;
    }
    int getDebugMode() const
    {
        return debug_mode_;
    }
    int getInnerIterationNum() const
    {
        return inner_iteration_num_;
    }
    int getOutterIterationNum() const
    {
        return outter_iteration_num_;
    }
    QVector3D getInitOffset() const
    {
        return init_offset_;
    }
    double getMaxDepth() const
    {
        return max_depth_;
    }
    double getMinDepth() const
    {
        return min_depth_;
    }
    double getDistTrunc() const
    {
        return distance_trunc_;
    }
    double getScoreThreshold() const
    {
        return score_threshold_;
    }
    double getPackMinval() const
    {
        return pack_minval_;
    }
    double getFeatureScoreThreshold() const
    {
        return feature_score_threshold_;
    }
    int getSampleAreaExpandLevel() const;
    double getSampleAreaIOUThreshold() const;


    QMatrix4x4 getDefaultViewMatrix() const;
    static const int DtSize = 16;


private:
    QMap<QString, TargetInfo> models_;
    QList<TargetInfo> targets_;

    RigidTransform default_view_;


    QString project_name_;

    QVector3D init_offset_{0, 0, -18};
    int canvas_size_{2048};
    int patch_number_{32};
    int bk_cost_{40};
    double max_depth_{200};
    double min_depth_{30};
    double score_threshold_{2.6};
    double feature_score_threshold_{-1};
    double distance_trunc_{5.0};
    double pack_minval_{0.0};
    int debug_mode_{0};
    int buffer_width_{1088};
    int buffer_height_{870};
    int query_num_once_{16};
    int polymap_width_{1088};
    int polymap_height_{870};

    int inner_iteration_num_{10};
    int outter_iteration_num_{4};

    int boundary_extendlevel_{16};
    int part_leastsize_{100};
    int samplearea_expand_level_{5};
    double samplearea_iou_threshold_{0.8};
};
}
#endif // QFINDERCONFIG_H
