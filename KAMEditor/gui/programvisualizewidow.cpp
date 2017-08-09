#include "programvisualizewidow.h"
#include "ui_programvisualizewidow.h"

ProgramVisualizeWidow::ProgramVisualizeWidow(QWidget *parent, bool _run) :
    QDialog(parent),
    ui(new Ui::ProgramVisualizeWidow),
    run(_run)
{
    ui->setupUi(this);
}

ProgramVisualizeWidow::~ProgramVisualizeWidow()
{
    delete ui;
}

void ProgramVisualizeWidow::on_zoomInPushButton_clicked()
{
    ui->programOpenGLWidget->scaling(1);
}

void ProgramVisualizeWidow::on_zoomOutPushButton_clicked()
{
    ui->programOpenGLWidget->scaling(-1);
}

void ProgramVisualizeWidow::on_topPushButton_clicked()
{
    ui->programOpenGLWidget->setXAngle(0);
    ui->programOpenGLWidget->setYAngle(0);
    ui->programOpenGLWidget->setZAngle(0);
    ui->programOpenGLWidget->rotate();
}
