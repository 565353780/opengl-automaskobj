#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "OpenGL_Auto_Obj_Masker/OpenGL_Auto_Obj_Masker.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void run_example();

private:
    Ui::MainWindow *ui;

    OpenGL_Auto_Obj_Masker opengl_auto_obj_masker_;
};
#endif // MAINWINDOW_H
