#include "QMesh3D.h"
#include  "QMaterial.h"
#include <iostream>
#include <QFileInfo>
#include <ctime>
#include "QMathUtil.h"
#include "QMeshParser.h"
namespace GCL
{

QMesh3D::QMesh3D(QObject *parent) : QObject(parent)
{
    this->initializeOpenGLFunctions();
    this->array_buffer_.create();
    this->index_buffer_.create();

}

QMesh3D::QMesh3D(QMaterial *material, QObject *parent): QObject(parent),
    material_(material)
{
    this->initializeOpenGLFunctions();
    this->array_buffer_.create();
    this->index_buffer_.create();

}

QMesh3D::~QMesh3D()
{
    if(array_buffer_.isCreated())
    {
        array_buffer_.destroy();
    }

    if(index_buffer_.isCreated())
    {
        index_buffer_.destroy();
    }
}

void QMesh3D::loadFile(const QString &filename, bool flat)
{
    std::vector<VertexData> array_data;
    std::vector<FaceData> face_data;
    QTriMeshParser::loadFile(filename, array_data, face_data, flat);

    updateArrayBuffer(array_data);
    updateIndexBuffer(face_data, 0, GL_TRIANGLES);


}

void QMesh3D::updateArrayBuffer(const std::vector<QMesh3D::VertexData> &array_data)
{
    this->array_buffer_.bind();
    int vsize = array_data.size() * sizeof(VertexData);
    bbox_vmin_ = QVector3D(1.0, 1.0, 1.0) * 999999;
    bbox_vmax_ = -bbox_vmin_;
    for(const auto &data : array_data)
    {
        if(fabs(data.position_[0]) > 9999) continue;
        if(fabs(data.position_[1]) > 9999) continue;
        if(fabs(data.position_[2]) > 9999) continue;
        bbox_vmin_ = QMathUtil::minValue(bbox_vmin_, data.position_);
        bbox_vmax_ = QMathUtil::maxValue(bbox_vmax_, data.position_);
    }
    has_boundingbox_ = (bbox_vmax_.x() >= bbox_vmin_.x());
    this->array_buffer_.allocate(array_data.data(), vsize);

    verteice_size_ = array_data.size();
    if(has_savedata_)
    {
        vertices_ = array_data;
    }
    n_array_size_ = vsize;
}

void QMesh3D::updateIndexBuffer(const std::vector<QMesh3D::FaceData> &index_data, const uint &element_size, const GLenum &type)
{
    this->index_buffer_.bind();
    int fsize = sizeof(FaceData) * index_data.size();
    this->index_buffer_.allocate(index_data.data(), fsize);
    elements_size_ = (element_size == 0) ? index_data.size() : element_size;
    element_type_ = type;
}

void QMesh3D::updateIndexBuffer(const QMesh3D::FaceData *data, const uint &element_size, const GLenum &type)
{
    this->index_buffer_.bind();
    this->index_buffer_.allocate(data, element_size * sizeof(FaceData));

    elements_size_ = element_size;
    element_type_ = type;
}

QOpenGLShaderProgram *QMesh3D::getProgram()
{
    return material_;
}

void QMesh3D::setMaterial(QMaterial *program)
{
    material_ = program;
}

void QMesh3D::init()
{

    if(!material_)
    {
        this->initDefaultShader();
    }

}

void QMesh3D::render(const QMatrix4x4 &project_matrix, const QMatrix4x4 &model_matrix)
{
    if(!material_ || !material_->isLinked())
    {
        return;
    }
    if(array_buffer_.size() == 0) return;
    this->material_->bind();
    material_->setUniformValue("u_color", u_color_);
    material_->setUniformValue("u_selected", int(is_selected_));
    if(this->isBackGround())
    {
        material_->setUniformValue("mvp_matrix", QMatrix4x4());
        material_->setUniformValue("model_matrix", QMatrix4x4());
        material_->setUniformValue("transform_matrix", QMatrix4x4());
    }
    else
    {
        this->transform_matrix_.setToIdentity();
        this->transform_matrix_.translate(offset_);
        this->transform_matrix_.rotate(rotate_euler_[0], QVector3D(1, 0, 0));
        this->transform_matrix_.rotate(rotate_euler_[1], QVector3D(0, 1, 0));
        this->transform_matrix_.rotate(rotate_euler_[2], QVector3D(0, 0, 1));
        QMatrix4x4 scale_matrix;
        scale_matrix.scale(scale_);

        // scale_matrix 不会传递给儿子节点
        QMatrix4x4 _modelmatrix = model_matrix * this->transform_matrix_ * scale_matrix;
        QMatrix4x4 mvp_matrix = project_matrix * _modelmatrix;
        material_->setUniformValue("model_matrix", _modelmatrix);
        material_->setUniformValue("mvp_matrix", mvp_matrix);
        material_->setUniformValue("transform_matrix", this->transform_matrix_);


    }
    int active_teture_count = 0;
    this->material_->setUniforms(active_teture_count);


    // Tell OpenGL which VBOs to use
    this->index_buffer_.bind();
    this->array_buffer_.bind();
//    qDebug()<<this->array_buffer_.size()<<" "<<this->index_buffer_.size();


    quintptr offset = 0;

    int vertexLocation = material_->attributeLocation("a_position");
    material_->enableAttributeArray(vertexLocation);
    material_->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

    // Offset for texture coordinate
    offset += sizeof(QVector3D);

    int normalLocation = material_->attributeLocation("a_normal");
    material_->enableAttributeArray(normalLocation);
    material_->setAttributeBuffer(normalLocation, GL_FLOAT, offset, 3, sizeof(VertexData));
    offset += sizeof(QVector3D);

    int colorLocation = material_->attributeLocation("a_color");
    material_->enableAttributeArray(colorLocation);
    material_->setAttributeBuffer(colorLocation, GL_FLOAT, offset, 4, sizeof(VertexData));
    offset += sizeof(QVector4D);

    int texcoordLocation = material_->attributeLocation("a_texcoord");
    material_->enableAttributeArray(texcoordLocation);
    material_->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(VertexData));
    // Draw cube geometry using indices from VBO 1


//    qDebug()<<this->array_buffer_.size()<<" "<<this->index_buffer_.size();
    // Offset for position
    glDrawElements(element_type_, elements_size_, GL_UNSIGNED_INT, 0);
//    glDrawArrays(GL_POINTS,0,this->array_buffer_.size() / sizeof(VertexData));
//    glDrawArrays(GL_TRIANGLES,0,24);



}

void QMesh3D::setMatrixIdentidy()
{
    transform_matrix_.setToIdentity();
}


void QMesh3D::rotate(const QQuaternion &quat)
{
    transform_matrix_.rotate(quat);
}

void QMesh3D::rotateEuler(const QVector3D &eular)
{
    rotate_euler_ += eular;
//    applyLeftEuler(eular);
    for(int i = 0; i < 3; i++)
    {
        if(rotate_euler_[i] > 360)
        {
            rotate_euler_[i] -= 360;
        }
        if(rotate_euler_[i] < -360)
        {
            rotate_euler_[i] += 360;
        }
    }
}

void QMesh3D::rotate(float angle, const QVector3D &axis)
{

    transform_matrix_.rotate(angle, axis);
}

void QMesh3D::applyLeftEuler(const QVector3D &euler)
{
    QMatrix4x4 matrix;
    matrix.rotate(euler[0], QVector3D(1, 0, 0));
    matrix.rotate(euler[1], QVector3D(0, 1, 0));
    matrix.rotate(euler[2], QVector3D(0, 0, 1));
    matrix.rotate(rotate_euler_[0], QVector3D(1, 0, 0));
    matrix.rotate(rotate_euler_[1], QVector3D(0, 1, 0));
    matrix.rotate(rotate_euler_[2], QVector3D(0, 0, 1));

    this->rotate_euler_ = QMathUtil::fromMatrixToEuler(matrix, "ZYX");
}

void QMesh3D::applyRightEuler(const QVector3D &euler)
{
    QMatrix4x4 matrix;
    matrix.rotate(rotate_euler_[0], QVector3D(1, 0, 0));
    matrix.rotate(rotate_euler_[1], QVector3D(0, 1, 0));
    matrix.rotate(rotate_euler_[2], QVector3D(0, 0, 1));

    matrix.rotate(euler[0], QVector3D(1, 0, 0));
    matrix.rotate(euler[1], QVector3D(0, 1, 0));
    matrix.rotate(euler[2], QVector3D(0, 0, 1));


    this->rotate_euler_ = QMathUtil::fromMatrixToEuler(matrix, "ZYX");
}

void QMesh3D::translate(const QVector3D &offset)
{
    offset_ += offset;
}

QVector3D QMesh3D::localRotate(const QVector3D &v) const
{
    QMatrix4x4 matrix;
    matrix.rotate(rotate_euler_[0], QVector3D(1, 0, 0));
    matrix.rotate(rotate_euler_[1], QVector3D(0, 1, 0));
    matrix.rotate(rotate_euler_[2], QVector3D(0, 0, 1));
    return matrix.map(v);
}

QVector3D QMesh3D::localTransform(const QVector3D &v) const
{
    return localRotate(v) + offset_;
}

QVector3D QMesh3D::inverseLocalRotate(const QVector3D &v) const
{
    QMatrix4x4 matrix;
    matrix.rotate(-rotate_euler_[2], QVector3D(0, 0, 1));
    matrix.rotate(-rotate_euler_[1], QVector3D(0, 1, 0));
    matrix.rotate(-rotate_euler_[0], QVector3D(1, 0, 0));
    return matrix.map(v);
}

QVector3D QMesh3D::inverseLocalTransform(const QVector3D &v) const
{
    return  inverseLocalRotate(v - offset_);
}

void QMesh3D::initShader(const QString &vshader_filename, const QString &fshader_filename)
{
    if(material_)
    {

        if(material_->parent() == this)
        {
            delete material_;
            material_ = nullptr;
        }
    }
    material_ = new QMaterial(this);
    bool has_error = false;
    if(!has_error && !material_->addShaderFromSourceFile(QOpenGLShader::Vertex, vshader_filename))
    {
        has_error = true;
    }
    // Compile fragment shader
    if (!has_error && !material_->addShaderFromSourceFile(QOpenGLShader::Fragment, fshader_filename))
    {
        has_error = true;
    }
    if (!has_error && !material_->link())
    {
        has_error = true;
    }
    if(has_error)
    {
        material_->deleteLater();
        material_ = nullptr;
    }
}


void QMesh3D::initDefaultShader()
{
    if(material_) return;
    this->initShader(default_vshader_filename_, default_fshader_filename_);
}

bool QMesh3D::getHasSavedata() const
{
    return has_savedata_;
}

void QMesh3D::setHasSavedata(bool t)
{
    has_savedata_ = t;
}

GLenum QMesh3D::getGL_ElementType() const
{
    return element_type_;
}


QVector3D QMesh3D::get_bbox_vmax() const
{
    return bbox_vmax_;
}

void QMesh3D::getTransformedBBox(QVector3D &vmin, QVector3D &vmax) const
{

    QVector3D t0 = localTransform(bbox_vmin_);
    QVector3D t1 = localTransform(bbox_vmax_);

    vmax = QMathUtil::maxValue(t0, t1);
    vmin = QMathUtil::minValue(t0, t1);

}

QVector3D QMesh3D::get_bbox_vmin() const
{
    return bbox_vmin_;
}

void QMesh3D::setColor(const QVector4D &color)
{
    u_color_ = color;
}

void QMesh3D::writeVertex(int vid, const QMesh3D::VertexData &data)
{
    if(vid < 0 || vid * sizeof(VertexData) >= array_size()) return;
    array_buffer_.bind();
    array_buffer_.write(vid * sizeof(VertexData), &data, sizeof(VertexData));
    if(has_savedata_)
    {
        vertices_[vid] = data;
    }
}

QMesh3D::VertexData QMesh3D::readVertex(int vid)
{
    if(vid < 0 || vid * sizeof(VertexData) >= array_size()) return VertexData();
    if(has_savedata_)
    {
        return vertices_[vid];
    }

    VertexData data;
    array_buffer_.bind();
    array_buffer_.read(vid * sizeof(VertexData), &data, sizeof(VertexData));
//    array_buffer_.release();
    return data;
}

bool QMesh3D::writeVertices(const std::vector<QMesh3D::VertexData> &array_data)
{
    int tsize = array_data.size() * sizeof(VertexData);
    if(array_buffer_.size() < tsize) tsize = array_buffer_.size();
    array_buffer_.bind();
    array_buffer_.write(0, array_data.data(), tsize);
    if(has_savedata_)
    {
        vertices_ = array_data;
    }
    return true;

}

bool QMesh3D::readVertices(std::vector<QMesh3D::VertexData> &array_data)
{
    if(has_savedata_)
    {
        array_data = vertices_;
        return true;
    }

    array_data.resize(array_buffer_.size() / sizeof(VertexData));
    array_buffer_.bind();
    array_buffer_.read(0, array_data.data(), array_buffer_.size());
    return true;
}

bool QMesh3D::isVisible() const
{
    return is_visible_;
}

void QMesh3D::setVisible(bool t)
{
    is_visible_ = t;
}

int QMesh3D::array_size() const
{
    return n_array_size_;
}

}
