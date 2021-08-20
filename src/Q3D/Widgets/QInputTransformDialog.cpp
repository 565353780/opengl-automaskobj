#include "QInputTransformDialog.h"
#include "ui_qinputtransformdialog.h"

namespace GCL {

QInputTransformDialog::QInputTransformDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QInputTransformDialog)
{
    ui->setupUi(this);
    connect(ui->x_val_spinbox,SIGNAL(valueChanged(double)),this,SLOT(_slotValChange()));
    connect(ui->y_val_spinbox,SIGNAL(valueChanged(double)),this,SLOT(_slotValChange()));
    connect(ui->z_val_spinbox,SIGNAL(valueChanged(double)),this,SLOT(_slotValChange()));
    connect(ui->rx_val_spinbox,SIGNAL(valueChanged(double)),this,SLOT(_slotValChange()));
    connect(ui->ry_val_spinbox,SIGNAL(valueChanged(double)),this,SLOT(_slotValChange()));
    connect(ui->rz_val_spinbox,SIGNAL(valueChanged(double)),this,SLOT(_slotValChange()));


}

QInputTransformDialog::~QInputTransformDialog()
{
    delete ui;
}

void QInputTransformDialog::setTitle(const QString &name)
{
    this->setWindowTitle(name);
}

void QInputTransformDialog::showDialog(const QString &caption, QVector3D default_offset, QVector3D default_euler)
{
    is_ready_emit_signal_ = false;
    this->setTitle(caption);

    ui->x_val_spinbox->setValue(default_offset.x());
    ui->y_val_spinbox->setValue(default_offset.y());
    ui->z_val_spinbox->setValue(default_offset.z());

    ui->rx_val_spinbox->setValue(default_euler.x());
    ui->ry_val_spinbox->setValue(default_euler.y());
    ui->rz_val_spinbox->setValue(default_euler.z());

    offset_ = default_offset;
    euler_ = default_euler;

    is_ready_emit_signal_ = true;
    this->show();
}

void QInputTransformDialog::_slotValChange()
{
    if(is_ready_emit_signal_)
    {

        offset_.setX(ui->x_val_spinbox->value());
        offset_.setY(ui->y_val_spinbox->value());
        offset_.setZ(ui->z_val_spinbox->value());

        euler_.setX(ui->rx_val_spinbox->value());
        euler_.setY(ui->ry_val_spinbox->value());
        euler_.setZ(ui->rz_val_spinbox->value());

        emit this->signalValueChanged(offset_,euler_);
    }
}

QVector3D QInputTransformDialog::offset() const
{
    return offset_;
}

QVector3D QInputTransformDialog::euler() const
{
    return euler_;
}
}
