#include "OpenGL_Auto_Obj_Masker.h"
#include "qdebug.h"

    // QOpenGLFramebufferObject fbo(QSize(w,h),QOpenGLFramebufferObject::CombinedDepthStencil);
//
    // fbo.bind();
    // rw.render();
//
    // fbo.bindDefault();
    // qDebug()<<(clock() -t0)/(CLOCKS_PER_SEC/1000)<<"ms: Total";
    // fbo.toImage().save("testOff.png");
//    filename.replace(QString("ply"),QString("png"));
//    fbo.toImage(false).save(filename);

//    return QApplication::exec();

bool OpenGL_Auto_Obj_Masker::addNormalizedMesh(
    const QString &mesh_file_path,
    const QVector3D &center,
    const QVector3D &eular,
    const int &label_idx)
{
  EasyMesh *mesh = createNormalizedMeshWithPose(mesh_file_path, center, eular, label_idx);

  mesh_list_.emplace_back(mesh);

  return true;
}

bool OpenGL_Auto_Obj_Masker::clearMesh()
{
    for(size_t i = 0; i < mesh_list_.size(); ++i)
    {
        delete(mesh_list_[i]);
    }

    mesh_list_.clear();

    return true;
}

bool OpenGL_Auto_Obj_Masker::getMeshRect3D(
    const EasyMesh *mesh,
    std::vector<float> &rect_3d)
{
    std::vector<QVector3D> mesh_points;

    for(int i = 0; i < mesh->getVerticeSize(); ++i)
    {
        mesh_points.emplace_back(mesh->transform_matrix.map(mesh->vertex_list[i].position_));
    }

    float x_min = mesh_points[0].x();
    float x_max = x_min;
    float y_min = mesh_points[0].y();
    float y_max = y_min;
    float z_min = mesh_points[0].z();
    float z_max = z_min;

    for(int i = 1; i < mesh->getVerticeSize(); ++i)
    {
        x_min = fmin(x_min, mesh_points[i].x());
        x_max = fmax(x_max, mesh_points[i].x());
        y_min = fmin(y_min, mesh_points[i].y());
        y_max = fmax(y_max, mesh_points[i].y());
        z_min = fmin(z_min, mesh_points[i].z());
        z_max = fmax(z_max, mesh_points[i].z());
    }

    rect_3d.resize(6);
    rect_3d[0] = x_min;
    rect_3d[1] = y_min;
    rect_3d[2] = z_min;
    rect_3d[3] = x_max;
    rect_3d[4] = y_max;
    rect_3d[5] = z_max;

    return true;
}

bool OpenGL_Auto_Obj_Masker::setMeshPose(
    EasyMesh *mesh,
    const QVector3D &center,
    const QVector3D &eular)
{
    //mesh旋转
    mesh->setEuler(eular);

    //mesh平移
    mesh->setOffset(center);

    mesh->updateTransformMatrix();

    return true;
}

bool OpenGL_Auto_Obj_Masker::getMeshProjectPolygon(
    const EasyMesh *mesh,
    std::vector<float> &polygon)
{
    //设定标定rect初值
    float min_x = viewport_[2] - 1;
    float max_x = 0;
    float min_y = viewport_[3] - 1;
    float max_y = 0;

    for(int i = 0; i < mesh->getVerticeSize(); ++i)
    {
        std::vector<float> point_2d;

        getProjectPoint2D(mesh->transform_matrix, mesh->vertex_list[i].position_, point_2d);

        min_x = fmin(min_x, point_2d[0]);
        max_x = fmax(max_x, point_2d[0]);
        min_y = fmin(min_y, point_2d[1]);
        max_y = fmax(max_y, point_2d[1]);
    }

    //提高rect鲁棒性
    min_x = fmax(min_x, 0);
    max_x = fmin(max_x, viewport_[2] - 1);
    min_y = fmax(min_y, 0);
    max_y = fmin(max_y, viewport_[3] - 1);

    //存储rect为vector
    polygon.resize(4);
    polygon[0] = min_x;
    polygon[1] = min_y;
    polygon[2] = max_x;
    polygon[3] = max_y;

    return true;
}

bool OpenGL_Auto_Obj_Masker::getMeshProjectRects(
    std::vector<std::vector<float>> &project_rect_vec)
{
    project_rect_vec.resize(mesh_list_.size());

    for(size_t i = 0; i < mesh_list_.size(); ++i)
    {
        getMeshProjectRect(mesh_list_[i], project_rect_vec[i]);
    }

    return true;
}

bool OpenGL_Auto_Obj_Masker::saveImageAndLabel(
    const QString &output_dataset_dir,
    const QString &image_basename,
    const QString &label_name,
    const size_t &current_label_idx_in_image,
    const QString &data_type)
{
    //抓取屏幕当前界面并保存
    QPixmap pixmap = w_.grab();
    pixmap.save(output_dataset_dir + data_type + "/images/" + image_basename + ".jpg", "jpg");

    QImage image = pixmap.toImage();
    QImage mask_image = QImage(image.width(), image.height(), image.format());

    QRgb *line;

    for(int y = 0; y < image.height(); ++y)
    {
        line = (QRgb*)image.scanLine(y);

        for(int x = 0; x < image.width(); ++x)
        {
            if(qRed(line[x]) + qGreen(line[x]) + qBlue(line[x]) > 30)
            {
                mask_image.setPixel(x, y, qRgb(255, 255, 255));
            }
        }
    }

    mask_image.save(output_dataset_dir + data_type + "/annotations/" +
        image_basename + "_" + label_name + "_" + QString::number(current_label_idx_in_image) + ".jpg");

    return true;


    std::vector<std::vector<float>> project_rect_vec;

    getMeshProjectRects(project_rect_vec);

    std::vector<int> rect_list_used_idx;
    for(size_t i = 0; i < project_rect_vec.size(); ++i)
    {
        if(project_rect_vec[i][0] < project_rect_vec[i][2] && project_rect_vec[i][1] < project_rect_vec[i][3])
        {
            rect_list_used_idx.emplace_back(i);
        }
    }

    //创建labels序列
    QJsonArray json_labels;
    for(size_t i = 0; i < rect_list_used_idx.size(); ++i)
    {
        QJsonArray labelArray;
        labelArray.append(QString::number(mesh_list_[rect_list_used_idx[i]]->label_idx));

        json_labels.append(labelArray);
    }

    //创建polygons序列
    QJsonArray json_polygons;

    //创建polygon对应点序列并保存到polygons序列中
    for(size_t i = 0; i < rect_list_used_idx.size(); ++i)
    {
        QJsonArray polygonsArray;
        QJsonArray polygonArray;

        polygonArray.append(project_rect_vec[rect_list_used_idx[i]][0]);
        polygonArray.append(viewport_[3] - project_rect_vec[rect_list_used_idx[i]][3]);
        polygonsArray.append(polygonArray);

        polygonArray[1] = viewport_[3] - project_rect_vec[rect_list_used_idx[i]][1];
        polygonsArray.append(polygonArray);

        polygonArray[0] = project_rect_vec[rect_list_used_idx[i]][2];
        polygonsArray.append(polygonArray);

        polygonArray[1] = viewport_[3] - project_rect_vec[rect_list_used_idx[i]][3];
        polygonsArray.append(polygonArray);

        polygonArray[0] = project_rect_vec[rect_list_used_idx[i]][0];
        polygonsArray.append(polygonArray);

        json_polygons.append(polygonsArray);
    }

    //创建shape序列
    QJsonArray json_shape;
    json_shape.append(viewport_[2]);
    json_shape.append(viewport_[3]);

    //创建Area字典并将前面所有内容载入
    QJsonObject json_Area;
    json_Area.insert("labels", QJsonValue(json_labels));
    json_Area.insert("polygons", QJsonValue(json_polygons));
    json_Area.insert("shape", QJsonValue(json_shape));

    //创建总字典并将Area字典载入
    QJsonObject json;
    json.insert("Area", QJsonValue(json_Area));

    //创建json文件并将前面创建的字典转为字符串
    QJsonDocument document;
    document.setObject(json);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);
    QString strJson(byteArray);

    //保存json内容
    std::ofstream outfile((output_dataset_dir + "annotations/" + image_basename + ".json").toStdString());
    outfile << strJson.toStdString();

    outfile.close();

    return true;
}

bool OpenGL_Auto_Obj_Masker::Create_Dataset(
    const QString &source_dataset_path,
    const QString &output_dataset_dir,
    const size_t &data_width,
    const size_t &data_height)
{
    initEnv(data_width, data_height);

    QDir output_dataset_dir_(output_dataset_dir);
    if(output_dataset_dir_.exists())
    {
        output_dataset_dir_.removeRecursively();
    }
    output_dataset_dir_.mkpath(output_dataset_dir_.absolutePath());

    output_dataset_dir_.mkdir(output_dataset_dir + "train/");
    output_dataset_dir_.mkdir(output_dataset_dir + "val/");
    output_dataset_dir_.mkdir(output_dataset_dir + "train/annotations/");
    output_dataset_dir_.mkdir(output_dataset_dir + "train/images/");
    output_dataset_dir_.mkdir(output_dataset_dir + "val/annotations/");
    output_dataset_dir_.mkdir(output_dataset_dir + "val/images/");

    QDir dir;
    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    dir.setPath(source_dataset_path);

    QStringList model_class_folder_list = dir.entryList();

    std::vector<QVector3D> direction_vec;
    QVector3D new_direction;

    new_direction[0] = 0;
    new_direction[1] = 0;
    new_direction[2] = 0;
    direction_vec.emplace_back(new_direction);
    new_direction[0] = 90;
    new_direction[1] = 0;
    new_direction[2] = 0;
    direction_vec.emplace_back(new_direction);
    new_direction[0] = 180;
    new_direction[1] = 0;
    new_direction[2] = 0;
    direction_vec.emplace_back(new_direction);
    new_direction[0] = 270;
    new_direction[1] = 0;
    new_direction[2] = 0;
    direction_vec.emplace_back(new_direction);
    new_direction[0] = 0;
    new_direction[1] = 90;
    new_direction[2] = 0;
    direction_vec.emplace_back(new_direction);
    new_direction[0] = 0;
    new_direction[1] = 270;
    new_direction[2] = 0;
    direction_vec.emplace_back(new_direction);

    size_t min_val_direction_idx = 0.9 * direction_vec.size();

    size_t solved_class_num = 0;
    size_t solved_obj_num = 0;

    size_t current_image_idx = 0;

    for(QString model_class_folder_name : model_class_folder_list)
    {
        ++solved_class_num;
        solved_obj_num = 0;

        QString model_class_folder_path = source_dataset_path + model_class_folder_name;

        QFileInfoList model_file_list = GetFileList(model_class_folder_path);

        std::vector<EasyMesh *> easymesh_list;

        for(QFileInfo model_file : model_file_list)
        {
            ++solved_obj_num;
            ++current_image_idx;

            mesh_file_info_ = model_file;

            // if(mesh_file_info_.fileName().split(".")[1] == "obj")
            // {
                // qrand()
                // QVector3D position = QVector3D(0, 0, 2);
                // QVector3D rotation = QVector3D(0, 0, 0);
                // int label_idx = 0;
//
                // QString mesh_file_path = mesh_file_info_.absoluteFilePath();
                // addNormalizedMesh(mesh_file_path, position, rotation, label_idx);
//
                // EasyMesh2D mesh_2d;
                // getProjectMesh2D(mesh_list_.back(), mesh_2d);
//
                // std::vector<EasyMesh2D> mesh_2d_vec;
                // splitMesh2D(mesh_2d, mesh_2d_vec);
//
                // std::vector<EasyPolygon2D> polygon_vec;
                // getPolygonVec(mesh_2d_vec, polygon_vec);
//
                // qDebug() << "Solving at : " <<
                  // "Class : " << solved_class_num << "/" << model_class_folder_list.size() <<
                  // " Model : " << solved_obj_num << " / " << model_file_list.size();
            // }

            QVector3D position = QVector3D(0, 0, 2);
            QVector3D rotation;
            int label_idx = 0;
            
            if(mesh_file_info_.fileName().split(".")[1] == "obj")
            {
                for(size_t i = 0; i < direction_vec.size(); ++i)
                {
                    rotation = direction_vec[i];

                    QString mesh_file_path = mesh_file_info_.absoluteFilePath();
                    addNormalizedMesh(mesh_file_path, position, rotation, label_idx);

                    QString data_type = "train";
                    if(i >= min_val_direction_idx)
                    {
                        data_type = "val";
                    }
                    saveImageAndLabel(
                        output_dataset_dir,
                        QString::number(current_image_idx-1) + "direction" + QString::number(i),
                        model_class_folder_name,
                        0,
                        data_type);

                    clearMesh();

                    qDebug() << "Solving at :" <<
                      " Image idx : " << current_image_idx <<
                      " Class : " << solved_class_num << "/" << model_class_folder_list.size() <<
                      " Model : " << solved_obj_num << " / " << model_file_list.size() <<
                      " Direction : " << i+1 << " / " << direction_vec.size();
                }
            }
        }
    }

    //更新主窗口的显示
    w_.show();

//    QFile file("../Server_DataBase/train_dataset/darknet_dataset/my_labels.txt");

//    if(file.open(QIODevice::WriteOnly))
//    {
//        file.resize(0);

//        QTextStream labels_out(&file);

//        for(size_t i = 0; i < class_num; ++i)
//        {
//            labels_out << all_obj_file_list[true_obj_file_list[i]].fileName().split(".")[0] << endl;
//        }

//        file.close();
//    }
    return 1;
}

void OpenGL_Auto_Obj_Masker::setGLFormat()
{
    QSurfaceFormat format;
    if(QOpenGLContext::openGLModuleType() == QOpenGLContext::LibGL)
    {
        format.setRenderableType(QSurfaceFormat::OpenGL);
    }
    else
    {
        format.setRenderableType(QSurfaceFormat::OpenGLES);
        //surfaceFmt.setVersion(3,0);
    }
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setSamples(4);
    QSurfaceFormat::setDefaultFormat(format);
}

bool OpenGL_Auto_Obj_Masker::initEnv(
    const size_t &data_width,
    const size_t &data_height)
{
    //配置OpenGL
    setGLFormat();

    w_.setFixedSize(data_width, data_height);

    //显示
    w_.show();

    //创建虚拟场景
    scene_ = w_.getScene();

    //创建Cube材质和纹理
    material_ = new QMaterial(scene_);
    material_->linkShaders(":/shaders/simple_vshader.glsl",":/shaders/simple_fshader.glsl");

    //初始化相机位姿矩阵
    QMatrix4x4 matrix;
    matrix.lookAt(QVector3D(0,0,0),QVector3D(0,0,1),QVector3D(0,1,0));
    scene_->setDefaultModelMatrix(matrix);
    scene_->setDefaultView();

    //获取viewport参数
    glGetIntegerv(GL_VIEWPORT, viewport_);

    return true;
}

bool OpenGL_Auto_Obj_Masker::normalizeMesh(
    EasyMesh *mesh)
{
    std::vector<float> rect_3d;
    getMeshRect3D(mesh, rect_3d);

    //mesh归一化到单位立方体中
    float center_x = (rect_3d[0] + rect_3d[3]) / 2;
    float center_y = (rect_3d[1] + rect_3d[4]) / 2;
    float center_z = (rect_3d[2] + rect_3d[5]) / 2;

    float max_length = rect_3d[3] - rect_3d[0];
    max_length = fmax(max_length, rect_3d[4] - rect_3d[1]);
    max_length = fmax(max_length, rect_3d[5] - rect_3d[2]);

    for(int i = 0; i < mesh->getVerticeSize(); ++i)
    {
        mesh->vertex_list[i].position_.setX((mesh->vertex_list[i].position_.x() - center_x) / max_length);
        mesh->vertex_list[i].position_.setY((mesh->vertex_list[i].position_.y() - center_y) / max_length);
        mesh->vertex_list[i].position_.setZ((mesh->vertex_list[i].position_.z() - center_z) / max_length);
    }

    mesh->updateArrayBuffer(mesh->vertex_list);

    mesh->updateTransformMatrix();

    return true;
}

bool OpenGL_Auto_Obj_Masker::transformMesh(
    EasyMesh *mesh,
    const QVector3D &center,
    const QVector3D &eular)
{
    //mesh旋转
    mesh->rotateEuler(eular);

    //mesh平移
    mesh->translate(center);

    mesh->updateTransformMatrix();

    return true;
}

EasyMesh *OpenGL_Auto_Obj_Masker::createMesh(
    const QString &mesh_file_path,
    const int &label_idx)
{
    //申请新的mesh结构
    EasyMesh *mesh = new EasyMesh(material_, scene_);

    mesh->label_idx = label_idx;

    //加载obj到mesh
    mesh->loadFile(mesh_file_path);

    //添加mesh到场景中
    scene_->addModel(mesh);

    return mesh;
}

EasyMesh *OpenGL_Auto_Obj_Masker::createNormalizedMeshWithPose(
    const QString &mesh_file_path,
    const QVector3D &center,
    const QVector3D &eular,
    const int &label_idx)
{
    EasyMesh *mesh = createMesh(mesh_file_path, label_idx);

    normalizeMesh(mesh);

    transformMesh(mesh, center, eular);

    return mesh;
}

bool OpenGL_Auto_Obj_Masker::getProjectPoint2D(
    const QMatrix4x4 &transform_matrix,
    const QVector3D &point_3d,
    std::vector<float> &point_2d)
{
    point_2d.resize(2, -1);

    QVector3D mesh_point_3d_trans = transform_matrix.map(point_3d);

    QVector3D window_point_2d = scene_->project(mesh_point_3d_trans);

    point_2d[0] = viewport_[0] + (window_point_2d[0] + 1) * viewport_[2] / 2;
    point_2d[1] = viewport_[1] + (window_point_2d[1] + 1) * viewport_[3] / 2;

    return true;
}

bool OpenGL_Auto_Obj_Masker::getProjectMesh2D(
    const EasyMesh *mesh,
    EasyMesh2D &mesh_2d)
{
    for(const QMesh3D::VertexData &vertex : mesh->vertex_list)
    {
        std::vector<float> point_2d;
        getProjectPoint2D(mesh->transform_matrix, vertex.position_, point_2d);
        mesh_2d.addVertex(point_2d[0], point_2d[1]);
    }
    for(size_t i = 0; i < mesh->face_list.size() / 3; ++i)
    {
        mesh_2d.addFace(mesh->face_list[3 * i], mesh->face_list[3 * i + 1], mesh->face_list[3 * i + 2]);
    }

    for(size_t i = 0; i < mesh_2d.face_2d_list.size(); ++i)
    {
        mesh_2d.face_2d_list[i].neighboor_face_idx_vec.size();
    }

    return true;
}

bool OpenGL_Auto_Obj_Masker::findConnectedFace(
    const EasyMesh2D &mesh_2d,
    const std::vector<int> current_search_face_idx_vec,
    EasyMesh2D &sub_mesh_2d,
    std::vector<bool> &face_connected_vec,
    std::vector<int> &sub_vertex_real_idx_vec)
{
    std::vector<int> new_search_face_idx_vec;

    for(const int &current_face_idx : current_search_face_idx_vec)
    {
        for(const int &current_neighboor_face_idx :
            mesh_2d.face_2d_list[current_face_idx].neighboor_face_idx_vec)
        {
            if(!face_connected_vec[current_neighboor_face_idx])
            {
                std::vector<int> sub_face_vertex_idx_vec;

                for(const int &current_neighboor_face_vertex_idx :
                    mesh_2d.face_2d_list[current_neighboor_face_idx].vertex_idx_vec)
                {
                    bool sub_mesh_2d_have_this_face_vertex_idx = false;
                    for(size_t i = 0; i < sub_vertex_real_idx_vec.size(); ++i)
                    {
                        if(sub_vertex_real_idx_vec[i] == current_neighboor_face_vertex_idx)
                        {
                            sub_mesh_2d_have_this_face_vertex_idx = true;
                            sub_face_vertex_idx_vec.emplace_back(i);
                            break;
                        }
                    }

                    if(!sub_mesh_2d_have_this_face_vertex_idx)
                    {
                        sub_face_vertex_idx_vec.emplace_back(sub_mesh_2d.vertex_2d_list.size());
                        sub_vertex_real_idx_vec.emplace_back(current_neighboor_face_vertex_idx);

                        sub_mesh_2d.addVertex(
                            mesh_2d.vertex_2d_list[current_neighboor_face_vertex_idx].x,
                            mesh_2d.vertex_2d_list[current_neighboor_face_vertex_idx].y);
                    }
                }

                sub_mesh_2d.addFace(
                    sub_face_vertex_idx_vec[0],
                    sub_face_vertex_idx_vec[1],
                    sub_face_vertex_idx_vec[2]);

                face_connected_vec[current_neighboor_face_idx] = true;

                new_search_face_idx_vec.emplace_back(current_neighboor_face_idx);
            }
        }
    }

    if(new_search_face_idx_vec.size() == 0)
    {
        return true;
    }

    return findConnectedFace(
        mesh_2d,
        new_search_face_idx_vec,
        sub_mesh_2d,
        face_connected_vec,
        sub_vertex_real_idx_vec);
}

bool OpenGL_Auto_Obj_Masker::splitMesh2D(
    const EasyMesh2D &mesh_2d,
    std::vector<EasyMesh2D> &mesh_2d_vec)
{
    mesh_2d_vec.clear();

    size_t connected_face_num = 0;
    std::vector<bool> face_connected_vec;
    face_connected_vec.resize(mesh_2d.face_2d_list.size(), false);

    while(connected_face_num < mesh_2d.face_2d_list.size())
    {
        EasyMesh2D sub_mesh_2d;

        for(size_t i = 0; i < face_connected_vec.size(); ++i)
        {
            if(!face_connected_vec[i])
            {
                std::vector<int> current_search_face_idx_vec;
                current_search_face_idx_vec.emplace_back(i);

                std::vector<int> sub_vertex_real_idx_vec;

                for(const int &face_vertex_idx : mesh_2d.face_2d_list[i].vertex_idx_vec)
                {
                    sub_vertex_real_idx_vec.emplace_back(face_vertex_idx);

                    sub_mesh_2d.addVertex(
                        mesh_2d.vertex_2d_list[face_vertex_idx].x,
                        mesh_2d.vertex_2d_list[face_vertex_idx].y);
                }
                sub_mesh_2d.addFace(0, 1, 2);

                face_connected_vec[i] = true;

                findConnectedFace(
                    mesh_2d,
                    current_search_face_idx_vec,
                    sub_mesh_2d,
                    face_connected_vec,
                    sub_vertex_real_idx_vec);

                mesh_2d_vec.emplace_back(sub_mesh_2d);

                connected_face_num += sub_mesh_2d.face_2d_list.size();

                break;
            }
        }
    }

    return true;
}

bool OpenGL_Auto_Obj_Masker::getPolygonVec(
    const EasyMesh2D &mesh_2d,
    std::vector<EasyPolygon2D> &polygon_vec)
{
    polygon_vec.clear();

    if(mesh_2d.face_2d_list.size() == 0)
    {
        return true;
    }

    EasyMask2D mask;

    EasyPolygon2D current_polygon;

    for(const int face_vertex_idx : mesh_2d.face_2d_list[0].vertex_idx_vec)
    {
        EasyPoint2D face_vertex_point;
        face_vertex_point.setPosition(
            mesh_2d.vertex_2d_list[face_vertex_idx].x,
            mesh_2d.vertex_2d_list[face_vertex_idx].y);
        current_polygon.addPoint(face_vertex_point);
    }

    current_polygon.setAntiClockWise();

    if(mesh_2d.face_2d_list.size() == 1)
    {
        polygon_vec.emplace_back(current_polygon);

        return true;
    }

    std::vector<EasyPolygon2D> current_polygon_vec;
    current_polygon_vec.emplace_back(current_polygon);

    std::vector<bool> mesh_2d_face_connected_vec;
    mesh_2d_face_connected_vec.resize(mesh_2d.face_2d_list.size(), false);
    mesh_2d_face_connected_vec[0] = true;

    std::vector<size_t> current_search_face_idx_vec;
    current_search_face_idx_vec.emplace_back(0);

    bool face_neighboor_all_connected = false;

    std::vector<EasyPolygon2D> union_polygon_vec;

    while(!face_neighboor_all_connected)
    {
        face_neighboor_all_connected = true;

        std::vector<size_t> new_search_face_idx_vec;

        for(const size_t search_face_idx : current_search_face_idx_vec)
        {
            for(const size_t search_face_neighboor_face_idx :
                mesh_2d.face_2d_list[search_face_idx].neighboor_face_idx_vec)
            {
                if(!mesh_2d_face_connected_vec[search_face_neighboor_face_idx])
                {
                    face_neighboor_all_connected = false;
                    new_search_face_idx_vec.emplace_back(search_face_neighboor_face_idx);

                    EasyPolygon2D new_polygon;
                    for(const int search_face_neighboor_face_vertex_idx :
                        mesh_2d.face_2d_list[search_face_neighboor_face_idx].vertex_idx_vec)
                    {
                        EasyPoint2D face_vertex_point;
                        face_vertex_point.setPosition(
                            mesh_2d.vertex_2d_list[search_face_neighboor_face_vertex_idx].x,
                            mesh_2d.vertex_2d_list[search_face_neighboor_face_vertex_idx].y);
                        new_polygon.addPoint(face_vertex_point);
                    }

                    new_polygon.setAntiClockWise();
                    current_polygon_vec.emplace_back(new_polygon);

                    mask.getUnionPolygonVec(current_polygon_vec, union_polygon_vec);
                    current_polygon_vec = union_polygon_vec;
                }
            }
        }

        current_search_face_idx_vec = new_search_face_idx_vec;
    }

    std::cout << "=================================" << std::endl;
    std::cout << current_polygon_vec.size() << std::endl;

    return true;
}

bool OpenGL_Auto_Obj_Masker::getPolygonVec(
    const std::vector<EasyMesh2D> &mesh_2d_vec,
    std::vector<EasyPolygon2D> &polygon_vec)
{
    polygon_vec.resize(mesh_2d_vec.size());

    if(mesh_2d_vec.size() == 0)
    {
        return true;
    }

    for(size_t i = 0; i < mesh_2d_vec.size(); ++i)
    {
        std::vector<EasyPolygon2D> union_polygon_vec;
        getPolygonVec(mesh_2d_vec[i], union_polygon_vec);

        for(const EasyPolygon2D &union_polygon : union_polygon_vec)
        {
            polygon_vec.emplace_back(union_polygon);
        }
    }

    return true;
}

bool OpenGL_Auto_Obj_Masker::getMeshProjectRect(
    const EasyMesh *mesh,
    std::vector<float> &project_rect)
{
    //设定标定rect初值
    float min_x = viewport_[2] - 1;
    float max_x = 0;
    float min_y = viewport_[3] - 1;
    float max_y = 0;


    for(int i = 0; i < mesh->getVerticeSize(); ++i)
    {
        std::vector<float> point_2d;

        getProjectPoint2D(mesh->transform_matrix, mesh->vertex_list[i].position_, point_2d);

        min_x = fmin(min_x, point_2d[0]);
        max_x = fmax(max_x, point_2d[0]);
        min_y = fmin(min_y, point_2d[1]);
        max_y = fmax(max_y, point_2d[1]);
    }

    //提高rect鲁棒性
    min_x = fmax(min_x, 0);
    max_x = fmin(max_x, viewport_[2] - 1);
    min_y = fmax(min_y, 0);
    max_y = fmin(max_y, viewport_[3] - 1);

    project_rect.resize(4);
    project_rect[0] = min_x;
    project_rect[1] = min_y;
    project_rect[2] = max_x;
    project_rect[3] = max_y;

    return true;
}

QFileInfoList OpenGL_Auto_Obj_Masker::GetFileList(QString path)
{
    QDir dir(path);
    QFileInfoList file_list = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    QFileInfoList folder_list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

    for(int i = 0; i != folder_list.size(); i++)
    {
         QString name = folder_list.at(i).absoluteFilePath();
         QFileInfoList child_file_list = GetFileList(name);
         file_list.append(child_file_list);
    }

    return file_list;
}


bool OpenGL_Auto_Obj_Masker::cpDir(QString srcPath, QString dstPath)
{
    QDir parentDstDir(QFileInfo(dstPath).path());

    if(!parentDstDir.mkpath(QFileInfo(dstPath).fileName()))
    {
        return false;
    }

    QDir srcDir(srcPath);
    foreach(QFileInfo info, srcDir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot))
    {
        QString srcItemPath = srcPath + "/" + info.fileName();
        QString dstItemPath = dstPath + "/" + info.fileName();
        if(info.isDir())
        {
            if(!cpDir(srcItemPath, dstItemPath))
            {
                return false;
            }
        }
        else if(info.isFile())
        {
            if(!QFile::copy(srcItemPath, dstItemPath))
            {
                return false;
            }
        }
        else
        {
            qDebug() << "Unhandled item" << info.filePath() << "in cpDir";
        }
    }

    return true;
}

bool OpenGL_Auto_Obj_Masker::delDir(QString dirPath){
    QDir dir(dirPath);

    dir.removeRecursively();

    return true;
}

