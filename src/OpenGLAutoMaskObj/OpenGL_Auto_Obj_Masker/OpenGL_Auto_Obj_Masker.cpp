#include "OpenGL_Auto_Obj_Masker/OpenGL_Auto_Obj_Masker.h"
#include "qdebug.h"

bool EasyVertex2D::haveThisVertexNeighboor(
    const int &neighboor_vertex_idx)
{
    if(neighboor_vertex_idx_vec.size() == 0)
    {
        return false;
    }

    for(const int &exist_neighboor_vertex_idx : neighboor_vertex_idx_vec)
    {
        if(exist_neighboor_vertex_idx == neighboor_vertex_idx)
        {
            return true;
        }
    }

    return false;
}

bool EasyVertex2D::haveThisFaceNeighboor(
    const int &neighboor_face_idx)
{
    if(neighboor_face_idx_vec.size() == 0)
    {
        return false;
    }

    for(const int &exist_neighboor_face_idx : neighboor_face_idx_vec)
    {
        if(exist_neighboor_face_idx == neighboor_face_idx)
        {
            return true;
        }
    }

    return false;
}

bool EasyVertex2D::addVertexNeighboor(
    const int &neighboor_vertex_idx)
{
    if(haveThisVertexNeighboor(neighboor_vertex_idx))
    {
        return true;
    }
    neighboor_vertex_idx_vec.emplace_back(neighboor_vertex_idx);
    return true;
}

bool EasyVertex2D::addFaceNeighboor(
    const int &neighboor_face_idx)
{
    if(haveThisFaceNeighboor(neighboor_face_idx))
    {
        return true;
    }
    neighboor_face_idx_vec.emplace_back(neighboor_face_idx);
    return true;
}

bool EasyFace2D::setVertexIndex(
    const int &vertex_idx_1,
    const int &vertex_idx_2,
    const int &vertex_idx_3)
{
    vertex_idx_vec.resize(3);
    vertex_idx_vec[0] = vertex_idx_1;
    vertex_idx_vec[1] = vertex_idx_2;
    vertex_idx_vec[2] = vertex_idx_3;
    return true;
}

bool EasyFace2D::haveThisVertex(
    const int &vertex_idx)
{
    for(const int &face_vertex_idx : vertex_idx_vec)
    {
        if(face_vertex_idx == vertex_idx)
        {
            return true;
        }
    }

    return false;
}

bool EasyMesh2D::addVertex(
    const int &x,
    const int &y)
{
    EasyVertex2D vertex_2d;
    vertex_2d.x = x;
    vertex_2d.y = y;

    vertex_2d_list.emplace_back(vertex_2d);

    return true;
}

bool EasyMesh2D::addVertexNeighboor(
    const int &vertex_1_idx,
    const int &vertex_2_idx)
{
    if(vertex_1_idx < first_vertex_idx || vertex_1_idx >= vertex_2d_list.size() + first_vertex_idx)
    {
        std::cout << "Add Neighboor failed! Vertex 1 idx out of range" << std::endl;
        std::cout << "valid value must in [" <<
          0 << "," << vertex_2d_list.size() - first_vertex_idx <<
          "]" << std::endl;
        std::cout << "current value is " << vertex_1_idx << std::endl;
        return false;
    }
    if(vertex_2_idx < first_vertex_idx || vertex_2_idx >= vertex_2d_list.size() + first_vertex_idx)
    {
        std::cout << "Add Neighboor failed! Vertex 2 idx out of range" << std::endl;
        std::cout << "valid value must in [" <<
          0 << "," << vertex_2d_list.size() - first_vertex_idx <<
          "]" << std::endl;
        std::cout << "current value is " << vertex_2_idx << std::endl;
        return false;
    }

    vertex_2d_list[vertex_1_idx - first_vertex_idx].addVertexNeighboor(vertex_2_idx - first_vertex_idx);
    vertex_2d_list[vertex_2_idx - first_vertex_idx].addVertexNeighboor(vertex_1_idx - first_vertex_idx);

    return true;
}

bool EasyMesh2D::addFace(
    const int &vertex_idx_1,
    const int &vertex_idx_2,
    const int &vertex_idx_3)
{
    EasyFace2D face_2d;
    face_2d.setVertexIndex(
        vertex_idx_1 - first_vertex_idx,
        vertex_idx_2 - first_vertex_idx,
        vertex_idx_3 - first_vertex_idx);
    face_2d_list.emplace_back(face_2d);

    for(const int &face_vertex_idx : face_2d.vertex_idx_vec)
    {
        vertex_2d_list[face_vertex_idx].addFaceNeighboor(face_2d_list.size() - 1);
    }

    return true;
}

int OpenGL_Auto_Obj_Masker::testOffScreen(QString filename)
{
    using namespace GCL;
    QOpenGLContext ctx;;
    QSurfaceFormat surfaceFmt;
    if(QOpenGLContext::openGLModuleType() == QOpenGLContext::LibGL)
    {
        surfaceFmt.setRenderableType(QSurfaceFormat::OpenGL);
    }
    else
    {
        surfaceFmt.setRenderableType(QSurfaceFormat::OpenGLES);
        //surfaceFmt.setVersion(3,0);
    }
    ctx.setFormat(surfaceFmt);
    bool b = ctx.create();
    surfaceFmt.setDepthBufferSize(24);
    QSurfaceFormat::setDefaultFormat(surfaceFmt);

    QOffscreenSurface* pSurface = new QOffscreenSurface;
    pSurface->setFormat(surfaceFmt);
    pSurface->create();

    ctx.makeCurrent(pSurface);
    clock_t t0 = clock();
    int w,h;
    w = 1280 * 0.8;
    h = 1024 * 0.8;
    QPointMapModelFinder rw;

    rw.init();
    rw.resize(w,h);


    GCL::Q3DScene *scene = rw.getScene();

    // 设置相机矩阵
    QMatrix4x4 matrix;
    matrix.lookAt(QVector3D(0,0,0),QVector3D(0,0,1),QVector3D(0,1,0));
    matrix.translate(-613,-49,-216);

    scene_->setDefaultModelMatrix(matrix);
    scene_->setDefaultView();


    // 读入模型
    rw.loadModel("c3.obj");


    // 读入点云图
    rw.loadPointMapPLY(filename,w,h,true,800);



    // 设置得分阈值, 阈值以上保留
    rw.setScoreThreshold(1.6);

    rw.setBoundaryExtendLevel(6);

    rw.setBoundaryExtendLevel(14);
    rw.setPartLeastSize(100);
    rw.setDepthRange(758,600);
    rw.setScoreThreshold(2.2);
    //    finder.updateBuffer();
    rw.findModels();




    // 以下只是为了保存图片用
    QOpenGLFramebufferObject fbo(QSize(w,h),QOpenGLFramebufferObject::CombinedDepthStencil);

    fbo.bind();
    rw.render();

    fbo.bindDefault();
    qDebug()<<(clock() -t0)/(CLOCKS_PER_SEC/1000)<<"ms: Total";
    fbo.toImage().save("testOff.png");
//    filename.replace(QString("ply"),QString("png"));
//    fbo.toImage(false).save(filename);

//    return QApplication::exec();

    return 0;
}

bool OpenGL_Auto_Obj_Masker::initEnv()
{
    //配置OpenGL
    setGLFormat();

    //创建主显示窗口

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
    for(int i = 0; i < mesh_list_.size(); ++i)
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
    std::vector<int> &polygon)
{
    //设定标定rect初值
    int min_x = viewport_[2] - 1;
    int max_x = 0;
    int min_y = viewport_[3] - 1;
    int max_y = 0;

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
    std::vector<std::vector<int>> &project_rect_vec)
{
    project_rect_vec.resize(mesh_list_.size());

    for(int i = 0; i < mesh_list_.size(); ++i)
    {
        getMeshProjectRect(mesh_list_[i], project_rect_vec[i]);
    }

    return true;
}

bool OpenGL_Auto_Obj_Masker::saveImageAndLabel(
    const QString &output_name)
{
    //抓取屏幕当前界面并保存
    QPixmap pixmap = w_.grab();
    pixmap.save(output_name + ".jpg", "jpg");

    std::vector<std::vector<int>> project_rect_vec;

    getMeshProjectRects(project_rect_vec);

    std::vector<int> rect_list_used_idx;
    for(int i = 0; i < project_rect_vec.size(); ++i)
    {
        if(project_rect_vec[i][0] < project_rect_vec[i][2] && project_rect_vec[i][1] < project_rect_vec[i][3])
        {
            rect_list_used_idx.emplace_back(i);
        }
    }

    //创建labels序列
    QJsonArray json_labels;
    for(int i = 0; i < rect_list_used_idx.size(); ++i)
    {
        QJsonArray labelArray;
        labelArray.append(QString::number(mesh_list_[rect_list_used_idx[i]]->label_idx));

        json_labels.append(labelArray);
    }

    //创建polygons序列
    QJsonArray json_polygons;

    //创建polygon对应点序列并保存到polygons序列中
    for(int i = 0; i < rect_list_used_idx.size(); ++i)
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
    std::ofstream outfile(output_name.toStdString() + ".json");
    outfile << strJson.toStdString();

    outfile.close();

    return true;
}

bool OpenGL_Auto_Obj_Masker::Create_Dataset()
{
    qDebug() << "start create dataset !";

    initEnv();

    QString source_dataset_path = "/home/chli/3D_FRONT/output/";
    QDir output_dataset_dir("/home/chli/3D_FRONT/output_mask_dataset");
    if(!output_dataset_dir.exists())
    {
        output_dataset_dir.mkpath(output_dataset_dir.absolutePath());
    }

    QDir dir;
    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    dir.setPath(source_dataset_path);

    QStringList model_class_folder_list = dir.entryList();

    class_num_ = model_class_folder_list.size();
    qDebug() << "model class num : " << class_num_;

    int solved_obj_num = 0;

    for(QString model_class_folder_name : model_class_folder_list)
    {
        QString model_class_folder_path = source_dataset_path + model_class_folder_name;

        QFileInfoList model_file_list = GetFileList(model_class_folder_path);

        std::vector<EasyMesh *> easymesh_list;

        for(QFileInfo model_file : model_file_list)
        {
            mesh_file_info_ = model_file;
            if(mesh_file_info_.fileName().split(".")[1] == "obj")
            {
                //qrand()
                QVector3D position = QVector3D(0, 0, 2);
                QVector3D rotation = QVector3D(0, 0, 0);
                int label_idx = 0;

                QString mesh_file_path = mesh_file_info_.absoluteFilePath();
                addNormalizedMesh(mesh_file_path, position, rotation, label_idx);

                MyOpenMesh open_mesh;
                createOpenMesh(mesh_list_.back(), open_mesh);

                EasyMesh2D mesh_2d;
                getProjectMesh2D(mesh_list_.back(), mesh_2d);

                std::vector<EasyMesh2D> mesh_2d_vec;

                splitMesh2D(mesh_2d, mesh_2d_vec);

                std::vector<EasyPolygon> polygon_vec;

                getPolygonVec(mesh_2d_vec, polygon_vec);

                ++solved_obj_num;
                qDebug() << solved_obj_num << " / " << model_file_list.size();
            }

            QString output_file_path = output_dataset_dir.absolutePath() + "/" + QString::number(solved_obj_num);

            saveImageAndLabel(output_file_path);

            clearMesh();

            if(solved_obj_num > 4)
            {
                exit(0);
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

//        for(int i = 0; i < class_num; ++i)
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

bool OpenGL_Auto_Obj_Masker::createOpenMesh(
    const EasyMesh *mesh,
    MyOpenMesh &open_mesh)
{
    MyOpenMesh::VertexHandle vh[mesh->vertex_list.size()];

    for(int i = 0; i < mesh->vertex_list.size(); ++i)
    {
        std::vector<float> point_2d;
        getProjectPoint2D(mesh->transform_matrix, mesh->vertex_list[i].position_, point_2d);
        vh[i] = open_mesh.add_vertex(MyOpenMesh::Point( point_2d[0], point_2d[1], 0));
    }
    for(int i = 0; i < mesh->face_list.size() / 3; ++i)
    {
        std::vector<MyOpenMesh::VertexHandle> fvh;
        for(int j = 0; j < 3; ++j)
        {
            fvh.emplace_back(vh[3 * i + j]);
        }
        open_mesh.add_face(fvh);
    }

    QString output_file_path = "/home/chli/3D_FRONT/" + mesh_file_info_.fileName();

    saveOpenMesh(open_mesh, output_file_path);

    return true;
}

bool OpenGL_Auto_Obj_Masker::saveOpenMesh(
    const MyOpenMesh &open_mesh,
    const QString &output_file_path)
{
    try
    {
        if (!OpenMesh::IO::write_mesh(open_mesh, output_file_path.toStdString()))
        {
            std::cerr << "Cannot write mesh to file '" << output_file_path.toStdString() << "'" << std::endl;
            return false;
        }
    }
    catch (std::exception& x)
    {
        std::cerr << x.what() << std::endl;
        return false;
    }   
    return true;
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
    for(int i = 0; i < mesh->face_list.size() / 3; ++i)
    {
        for(int j = 0; j < 3; ++j)
        {
            int vertex_1_idx = mesh->face_list[3 * i + j];
            int vertex_2_idx = mesh->face_list[3 * i + ((j + 1) % 3)];
            mesh_2d.addVertexNeighboor(vertex_1_idx, vertex_2_idx);
        }
    }

    return true;
}

bool OpenGL_Auto_Obj_Masker::findConnectedVertex(
    const EasyMesh2D &mesh_2d,
    const std::vector<int> current_search_vertex_idx_vec,
    EasyMesh2D &sub_mesh_2d,
    std::vector<bool> &vertex_connected_vec,
    std::vector<int> &sub_vertex_real_idx_vec)
{
    std::vector<int> new_search_vertex_vec;
    for(const int &current_vertex_idx : current_search_vertex_idx_vec)
    {
        int current_sub_vertex_idx = -1;
        for(int i = 0; i < sub_vertex_real_idx_vec.size(); ++i)
        {
            if(sub_vertex_real_idx_vec[i] == current_vertex_idx)
            {
                current_sub_vertex_idx = i;
                break;
            }
        }
        for(const int &current_neighboor_vertex_idx : mesh_2d.vertex_2d_list[current_vertex_idx].neighboor_vertex_idx_vec)
        {
            if(!vertex_connected_vec[current_neighboor_vertex_idx])
            {
                sub_mesh_2d.addVertex(mesh_2d.vertex_2d_list[current_neighboor_vertex_idx].x, mesh_2d.vertex_2d_list[current_neighboor_vertex_idx].y);
                vertex_connected_vec[current_neighboor_vertex_idx] = true;

                new_search_vertex_vec.emplace_back(current_neighboor_vertex_idx);

                sub_vertex_real_idx_vec.emplace_back(current_neighboor_vertex_idx);

                sub_mesh_2d.addVertexNeighboor(current_sub_vertex_idx, sub_vertex_real_idx_vec.size() - 1);

                continue;
            }

            if(find(new_search_vertex_vec.cbegin(), new_search_vertex_vec.cend(), current_neighboor_vertex_idx) != new_search_vertex_vec.cend())
            {
                continue;
            }

            int current_neighboor_sub_vertex_idx = -1;
            for(int i = 0; i < sub_vertex_real_idx_vec.size(); ++i)
            {
                if(sub_vertex_real_idx_vec[i] == current_neighboor_vertex_idx)
                {
                    current_neighboor_sub_vertex_idx = i;
                    break;
                }
            }
            sub_mesh_2d.addVertexNeighboor(current_sub_vertex_idx, current_neighboor_sub_vertex_idx);
        }
    }

    if(new_search_vertex_vec.size() == 0)
    {
        return true;
    }

    return findConnectedVertex(mesh_2d, new_search_vertex_vec, sub_mesh_2d, vertex_connected_vec, sub_vertex_real_idx_vec);
}

bool OpenGL_Auto_Obj_Masker::splitMesh2D(
    const EasyMesh2D &mesh_2d,
    std::vector<EasyMesh2D> &mesh_2d_vec)
{
    mesh_2d_vec.clear();

    int connected_vertex_num = 0;
    std::vector<bool> vertex_connected_vec;
    vertex_connected_vec.resize(mesh_2d.vertex_2d_list.size(), false);

    while(connected_vertex_num < mesh_2d.vertex_2d_list.size())
    {
        EasyMesh2D sub_mesh_2d;

        for(int i = 0; i < vertex_connected_vec.size(); ++i)
        {
            if(!vertex_connected_vec[i])
            {
                sub_mesh_2d.addVertex(mesh_2d.vertex_2d_list[i].x, mesh_2d.vertex_2d_list[i].y);
                vertex_connected_vec[i] = true;

                std::vector<int> current_search_vertex_idx_vec;
                current_search_vertex_idx_vec.emplace_back(i);

                std::vector<int> sub_vertex_real_idx_vec;
                sub_vertex_real_idx_vec.emplace_back(i);

                findConnectedVertex(mesh_2d, current_search_vertex_idx_vec, sub_mesh_2d, vertex_connected_vec, sub_vertex_real_idx_vec);

                mesh_2d_vec.emplace_back(sub_mesh_2d);

                connected_vertex_num += sub_mesh_2d.vertex_2d_list.size();

                break;
            }
        }
    }

    return true;
}

bool OpenGL_Auto_Obj_Masker::getPolygon(
    const EasyMesh2D &mesh_2d,
    EasyPolygon &polygon)
{
    return true;
}

bool OpenGL_Auto_Obj_Masker::getPolygonVec(
    const std::vector<EasyMesh2D> &mesh_2d_vec,
    std::vector<EasyPolygon> &polygon_vec)
{
    polygon_vec.resize(mesh_2d_vec.size());

    for(int i = 0; i < mesh_2d_vec.size(); ++i)
    {
        getPolygon(mesh_2d_vec[i], polygon_vec[i]);
    }

    return true;
}

bool OpenGL_Auto_Obj_Masker::getMeshProjectRect(
    const EasyMesh *mesh,
    std::vector<int> &project_rect)
{
    //设定标定rect初值
    int min_x = viewport_[2] - 1;
    int max_x = 0;
    int min_y = viewport_[3] - 1;
    int max_y = 0;


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

