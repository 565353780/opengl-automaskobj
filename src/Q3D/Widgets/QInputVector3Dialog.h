#ifndef QINPUTVECTOR3DIALOG_H
#define QINPUTVECTOR3DIALOG_H

#include <QDialog>
#include "../q3d_global.h"
#include <QVector3D>
namespace Ui {
class QInputVector3Dialog;
}
namespace GCL {

class Q3DGCLSHARED_EXPORT QInputVector3Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit QInputVector3Dialog(QWidget *parent = 0);
    ~QInputVector3Dialog();

    void setTitle(const QString &name);

    void showDialog(const QString &caption = "", QVector3D default_val = QVector3D());
signals:
    void signalVectorChanged(QVector3D);

private slots:
    void _slotVectorValChange();
private:
    Ui::QInputVector3Dialog *ui;
    QVector3D v_;
    bool is_ready_emit_signal_{false};
};
}
#endif // QINPUTVECTOR3DIALOG_H
