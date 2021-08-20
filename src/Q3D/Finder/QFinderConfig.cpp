#include "QFinderConfig.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QDebug>
namespace GCL
{

QVector3D jsontov3(const QJsonArray &arr)
{
    QVector3D v;
    for(int i = 0; i < 3 && i < arr.size(); i++)
    {
        v[i] = arr[i].toDouble(0.0);
    }
    return v;
}
QJsonArray v3tojson(const QVector3D &v)
{
    QJsonArray arr;
    for(int i = 0; i < 3; i++)
    {
        arr.push_back(double(v[i]));
    }
    return arr;
}

QFinderConfig::QFinderConfig(QObject *parent): QObject(parent)
{

}

void QFinderConfig::loadFile(const QString &filename)
{
    QFile file(filename);
    if(!file.open(QFile::ReadOnly))
    {
        qDebug() << filename << " not found!";
        return;
    }
    QJsonDocument doc;
    doc = doc.fromJson(file.readAll());
    file.close();


    this->fromJson(doc.object());

}

void QFinderConfig::saveFile(const QString &filename)
{
    QJsonObject obj;
    this->toJson(obj);

    QJsonDocument doc(obj);
    QFile file(filename);
    if(!file.open(QFile::WriteOnly))
    {
        qDebug() << filename << " open failed";
        return;
    }

    file.write(doc.toJson(QJsonDocument::Indented));

    file.close();
}

void QFinderConfig::fromJson(const QJsonObject &object)
{
    //    qDebug()<<object;
    project_name_ = object.value("project_name").toString();
    canvas_size_ = object.value("canvas_size").toInt(2048);
    patch_number_ = object.value("patch_number").toInt(32);
    bk_cost_ = object.value("bk_cost").toInt(40);

    score_threshold_ = object.value("score_threshold").toDouble(score_threshold_);
    feature_score_threshold_ = object.value("feature_score_threshold").toDouble(feature_score_threshold_);
    distance_trunc_ = object.value("distance_trunc").toDouble(distance_trunc_);
    max_depth_ = object.value("max_depth").toDouble(max_depth_);
    min_depth_ = object.value("min_depth").toDouble(min_depth_);
    pack_minval_ = object.value("pack_minval").toDouble(pack_minval_);
    buffer_width_  = object.value("buffer_width").toInt(buffer_width_);
    buffer_height_ = object.value("buffer_height").toInt(buffer_height_);

    polymap_width_  = object.value("polymap_width").toInt(polymap_width_);
    polymap_height_ = object.value("polymap_height").toInt(polymap_height_);




    inner_iteration_num_ = object.value("inner_iteration_num").toInt(inner_iteration_num_);
    outter_iteration_num_ = object.value("outter_iteration_num").toInt(outter_iteration_num_);
    query_num_once_ = object.value("query_num_once").toInt(query_num_once_);

    QJsonObject sample_obj = object.value("sample_method").toObject();

    boundary_extendlevel_ = sample_obj.value("boundary_extendlevel").toInt(boundary_extendlevel_);
    part_leastsize_ = sample_obj.value("part_leastsize").toInt(part_leastsize_);

    samplearea_expand_level_ = sample_obj.value("area_expand_level").toInt(samplearea_expand_level_);
    samplearea_iou_threshold_ = sample_obj.value("area_iou_threshold").toDouble(samplearea_iou_threshold_);


    debug_mode_ = object.value("debug_mode").toInt(debug_mode_);
    QJsonArray view_arr = object.value("default_view").toArray();
    default_view_.fromJson(view_arr);


    QJsonArray ioffset_arr = object.value("init_offset").toArray();
    init_offset_ = jsontov3(ioffset_arr);
    models_.clear();
    targets_.clear();

    QJsonObject models_obj = object.value("models").toObject();
    QStringList modelnames =  models_obj.keys();
    foreach (QString name, modelnames)
    {
        QJsonObject mobj = models_obj.value(name).toObject();
        TargetInfo targetinfo;
        targetinfo.fromJson(mobj);
        models_[name] = targetinfo;
    }

    QJsonArray targetsArr = object.value("targets").toArray();
    targets_.clear();
    for(int i = 0; i < targetsArr.size(); i++)
    {
        QJsonObject tobj =  targetsArr[i].toObject();
        QString name = tobj.value("model").toString();
        if(models_.count(name) == 0)
        {
            qDebug() << "Model: " << name << " not found!";
            continue;
        }
        TargetInfo targetinfo = models_[name];
        targetinfo.fromJson(tobj);
        targetinfo.id_ = targets_.size();
        targets_.push_back(targetinfo);
    }
    //    qDebug()<<models_.size()<<" "<<targets_.size()<<" "<<targetsArr.size()<<" "<<modelnames.size();
}

void QFinderConfig::toJson(QJsonObject &object)
{
    object["project_name"] =  project_name_;
    object["canvas_size"] = canvas_size_;
    object["patch_number"] = patch_number_;
    object["bk_cost"] = bk_cost_;

    QJsonObject modelObj;
    auto names = models_.keys();
    for(auto name : names)
    {
        QJsonObject tobj;
        models_[name].toJson(tobj);
        modelObj[name] = tobj;
    }
    object["models"] = modelObj;

    QJsonArray targetsArr;
    for(int i = 0; i < targets_.size(); i++)
    {
        QJsonObject tobj;
        targets_[i].toJson(tobj);
        targetsArr.push_back(tobj);
    }
    object["targets"] = targetsArr;

}

QList<QString> QFinderConfig::modelnames() const
{
    return models_.keys();
}

QString QFinderConfig::getFilepath(const QString &name) const
{
    return models_.value(name).filepath_;
}

int QFinderConfig::getGraidentSize() const
{
    return 16;
}

int QFinderConfig::getMaxOnceQueryPointsNumber() const
{
    int tnum = patch_number_ * patch_number_ / 16 / targets_.size();
    if(query_num_once_ < tnum) tnum = query_num_once_;
    return tnum;
}

int QFinderConfig::getTargetSize() const
{
    return targets_.size();
}

QFinderConfig::TargetInfo *QFinderConfig::getTargetInfo(int i)
{
    if(i < 0 || i >= targets_.size()) return nullptr;
    return &targets_[i];
}

QString QFinderConfig::getProjectName() const
{
    return project_name_;
}

int QFinderConfig::getCanvasSize() const
{
    return canvas_size_;
}

int QFinderConfig::getPatchNumber() const
{
    return patch_number_;
}

int QFinderConfig::getBkCost() const
{
    return bk_cost_;
}

QMatrix4x4 QFinderConfig::getDefaultViewMatrix() const
{
    return default_view_.toMatrix();
}

int QFinderConfig::getSampleAreaExpandLevel() const
{
    return samplearea_expand_level_;
}

double QFinderConfig::getSampleAreaIOUThreshold() const
{
    return samplearea_iou_threshold_;
}

void QFinderConfig::TargetInfo::fromJson(const QJsonObject &obj)
{
    filepath_ = obj.value("path").toString(filepath_);
    modelname_ = obj.value("model").toString(modelname_);
    QJsonArray pickArr = obj.value("pickpoints").toArray();
    if(pickArr.size() > 0)
    {
        pickpoints_.clear();
        placepoints_.clear();
    }
    for(int i = 0; i < pickArr.size(); i++)
    {
        QJsonObject tobj = pickArr.at(i).toObject();
        QJsonArray pick = tobj.value("pick").toArray();

        RigidTransform pick_rt;
        pick_rt.fromJson(pick);
        pickpoints_.push_back(pick_rt);

        QJsonArray place = tobj.value("place").toArray();
        RigidTransform place_rt;
        place_rt.fromJson(place);
        placepoints_.push_back(place_rt);
    }

    QJsonArray featureArr = obj.value("featurepoints").toArray();
    if(featureArr.size() > 0)
    {
        featurepoints_.clear();
    }

    for(int i = 0; i < featureArr.size(); i++)
    {
        QJsonObject tobj = featureArr.at(i).toObject();
        QJsonArray point = tobj.value("point").toArray();
        RigidTransform feature_rt;
        feature_rt.fromJson(point);
        featurepoints_.push_back(feature_rt);

    }

    scissor_level_ = obj.value("scissor_level").toInt(scissor_level_);
    score_weight_ = obj.value("score_weight").toDouble(score_weight_);
    score_threhold_ = obj.value("score_threshold").toDouble(score_threhold_);
    offset_scale_ = obj.value("offset_scale").toDouble(offset_scale_);
    angle_scale_ = obj.value("angle_scale").toDouble(angle_scale_);

    QJsonArray dtArr = obj.value("dt").toArray();
    if(dtArr.size() > 0)
    {
        if(dtArr.size() != 16)
        {
            qDebug() << "dt size != 16 -->"  << dtArr.size();
        }
        else
        {
            dt_.clear();
            for(int i = 0; i < dtArr.size(); i++)
            {
                QJsonArray tarr = dtArr.at(i).toArray();
                RigidTransform rt;
                rt.fromJson(tarr);
                dt_.push_back(rt);
            }
        }
    }
    init_euler_ = jsontov3( obj.value("init_euler").toArray());
    refine_euler_ = jsontov3(obj.value("refine_euler").toArray());
}

void QFinderConfig::TargetInfo::toJson(QJsonObject &obj) const
{
    obj["path"] = filepath_;
    obj["model"] = modelname_;
    QJsonArray pickArr;
    for(int i = 0; i < pickpoints_.size(); i++)
    {
        QJsonObject tobj;

        QJsonArray pick;
        pickpoints_[i].toJson(pick);

        QJsonArray place;
        placepoints_[i].toJson(place);

        tobj["pick"] = pick;
        tobj["place"] = place;

        pickArr.push_back(tobj);
    }
    obj["pickpoints"] = pickArr;
    obj["angle_scale"] = angle_scale_;
    obj["offset_scale"] = offset_scale_;
    obj["scissor_level"] = scissor_level_;
    obj["init_euler"] = v3tojson(init_euler_);
    obj["refine_euler"] = v3tojson(refine_euler_);
    obj["score_threshold"] = score_threhold_;
    obj["score_weight"] = score_weight_;

    QJsonArray dtArr;
    for(int i = 0; i < dt_.size(); i++)
    {
        QJsonArray tarr;
        dt_[i].toJson(tarr);
        dtArr.push_back(tarr);
    }
    obj["dt"] = dtArr;

}



bool QFinderConfig::RigidTransform::fromJson(const QJsonArray &arr)
{
    if(arr.size() < 6) return false;
    for(int i = 0; i < 3; i++)
    {
        offset_[i] = arr.at(i).toDouble(0.0);
    }
    for(int i = 0; i < 3; i++)
    {
        euler_[i] = arr.at(3 + i).toDouble(0.0);
    }
    return false;


}
double toFixed2(double x)
{
    return int(x * 100 + 0.5) / 100.0;
}
void QFinderConfig::RigidTransform::toJson(QJsonArray &arr) const
{
    arr = QJsonArray();
    arr.push_back(toFixed2(offset_[0]));
    arr.push_back(toFixed2(offset_[1]));
    arr.push_back(toFixed2(offset_[2]));
    arr.push_back(toFixed2(euler_[0]));
    arr.push_back(toFixed2(euler_[1]));
    arr.push_back(toFixed2(euler_[2]));
}

QMatrix4x4 QFinderConfig::RigidTransform::toMatrix() const
{
    QMatrix4x4 matrix;
    matrix.translate(offset_);

    matrix.rotate(euler_[0], QVector3D(1, 0, 0));
    matrix.rotate(euler_[1], QVector3D(0, 1, 0));
    matrix.rotate(euler_[2], QVector3D(0, 0, 1));

    return matrix;

}

QVector4D QFinderConfig::RigidTransform::toDt4() const
{
    if(offset_.length() > 0)
    {
        return QVector4D(offset_.x(), offset_.y(), offset_.z(), 0.0);
    }
    else
    {
        return QVector4D(euler_.x(), euler_.y(), euler_.z(), 1.0);
    }
}

}
