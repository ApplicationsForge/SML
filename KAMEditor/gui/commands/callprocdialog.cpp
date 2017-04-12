#include "callprocdialog.h"
#include "ui_callprocdialog.h"

CallProcDialog::CallProcDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CallProcDialog)
{
    ui->setupUi(this);

    std::vector<QLineEdit*> fields =
    {
         ui->call_name_lineEdit
    };
    fillFields(fields);
}

CallProcDialog::~CallProcDialog()
{
    delete ui;
}

void CallProcDialog::on_buttonBox_accepted()
{

    Command cmd;
    cmd.id = CMD_CALL;

    //cmd.commandColor = "#2f4f4f";
    cmd.commandColor = "#4682B4";


    std::string name = ui->call_name_lineEdit->text().toStdString();

    cmd.args = {
     name,
    };
    setCommandArguments(cmd);
}
