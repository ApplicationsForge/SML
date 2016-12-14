#include "labeldialog.h"
#include "ui_labeldialog.h"

LabelDialog::LabelDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LabelDialog)
{
    ui->setupUi(this);
}

LabelDialog::~LabelDialog()
{
    delete ui;
}

void LabelDialog::on_buttonBox_accepted()
{
    Command cmd;
    cmd.id = CMD_LABEL;



    std::string name = ui->label_lineEdit_name->text().toStdString();

    cmd.args = {
     name,
    };
    CommandInterpreter::Instance().addCommand(cmd);
}