#include "linedialog.h"
#include "ui_linedialog.h"

LineDialog::LineDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LineDialog)
{
    ui->setupUi(this);
}

LineDialog::~LineDialog()
{
    delete ui;
}

void LineDialog::on_buttonBox_accepted()
{
    Command cmd;
    cmd.id = CMD_LINE;


    Vector v(
    ui->line_lineEdit_axis_x->text().toDouble(),
    ui->line_lineEdit_axis_y->text().toDouble(),
    ui->line_lineEdit_axis_z->text().toDouble(),
    ui->line_lineEdit_axis_a->text().toDouble(),
    ui->line_lineEdit_axis_b->text().toDouble()
    );
    int velocity = ui->line_lineEdit_velocity->text().toInt();

    cmd.args = {
     std::to_string(v.x),
     std::to_string(v.y),
     std::to_string(v.z),
     std::to_string(v.a),
     std::to_string(v.b),
     std::to_string(velocity),
    };
    CommandInterpreter::Instance().addCommand(cmd);
}