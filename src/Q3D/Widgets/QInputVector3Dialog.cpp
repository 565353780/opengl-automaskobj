#include "QInputVector3Dialog.h"
#include "ui_qinputvector3dialog.h"
#include <QDebug>
namespace GCL {

QInputVector3Dialog::QInputVector3Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QInputVector3Dialog)
{
    ui->setupUi(this);
    connect(ui->xval_spinbox_,SIGNAL(valueChanged(double)),this,SLOT(_slotVectorValChange()));
    connect(ui->yval_spinbox_,SIGNAL(valueChanged(double)),this,SLOT(_slotVectorValChange()));
    connect(ui->zval_spinbox_,SIGNAL(valueChanged(double)),this,SLOT(_slotVectorValChange()));
}


QInputVector3Dialog::~QInputVector3Dialog()
{
    delete ui;
}

void QInputVector3Dialog::setTitle(const QString &name)
{
    this->setWindowTitle(name);
}

void QInputVector3Dialog::showDialog(const QString &caption, QVector3D default_val)
{
    this->setTitle(caption);
    is_ready_emit_signal_ = false;
    ui->xval_spinbox_->setValue(default_val.x());
    ui->yval_spinbox_->setValue(default_val.y());
    ui->zval_spinbox_->setValue(default_val.z());
    v_ = default_val;
    is_ready_emit_signal_ = true;
    this->show();
}

void QInputVector3Dialog::_slotVectorValChange()
{
    if(is_ready_emit_signal_)
    {

        v_.setX(ui->xval_spinbox_->value());
        v_.setY(ui->yval_spinbox_->value());
        v_.setZ(ui->zval_spinbox_->value());


        emit this->signalVectorChanged(v_);
    }

}
}
