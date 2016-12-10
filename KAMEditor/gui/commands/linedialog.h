#ifndef LINEDIALOG_H
#define LINEDIALOG_H

#include <QDialog>

#include "commandinterpreter.h"

namespace Ui {
class LineDialog;
}

class LineDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LineDialog(QWidget *parent = 0);
    ~LineDialog();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::LineDialog *ui;
};

#endif // LINEDIALOG_H