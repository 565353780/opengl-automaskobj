#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    run_example();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::run_example()
{
    QString source_dataset_path = "/home/chli/3D_FRONT/output/";
    QString output_dataset_dir = "/home/chli/3D_FRONT/output_mask_1920x1080/";
    size_t data_width = 1920;
    size_t data_height = 1080;

    opengl_auto_obj_masker_.Create_Dataset(
        source_dataset_path,
        output_dataset_dir,
        data_width,
        data_height);
}
