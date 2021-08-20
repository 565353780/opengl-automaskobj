#ifndef QINPUTTRANSFORMDIALOG_H
#define QINPUTTRANSFORMDIALOG_H

#include <QDialog>
#include <QVector3D>
#include "../q3d_global.h"
namespace Ui {
class QInputTransformDialog;
}
namespace GCL {

class Q3DGCLSHARED_EXPORT QInputTransformDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QInputTransformDialog(QWidget *parent = 0);
    ~QInputTransformDialog();


    void setTitle(const QString &name);
    void showDialog(const QString &caption = "", QVector3D default_offset = QVector3D(), QVector3D default_euler = QVector3D());
signals:
    void signalValueChanged(QVector3D, QVector3D);

private slots:
    void _slotValChange();

    QVector3D offset() const;
    QVector3D euler() const;

private:
    Ui::QInputTransformDialog *ui;


    QVector3D offset_;
    QVector3D euler_;
    bool is_ready_emit_signal_{false};
};
}
#endif // QINPUTTRANSFORMDIALOG_H
