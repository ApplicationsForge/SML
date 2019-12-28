#ifndef PROGRAM_PROCESSING_DIALOG_H
#define PROGRAM_PROCESSING_DIALOG_H

#include <QDialog>

#include "models/machine_tool.h"
#include "models/services/program/prepare_execution_queue_interactor.h"

namespace Ui {
class ProgramProcessingDialog;
}

class ProgramProcessingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProgramProcessingDialog(QWidget *parent = nullptr);
    ~ProgramProcessingDialog();

    void startProgramProcessing();

    void keyPressEvent(QKeyEvent *event);

private:
    Ui::ProgramProcessingDialog *ui;
    bool m_paused;

    void setupWidgets();
    void setupConnections();
    void resetConnections();

private slots:
    void onMachineTool_WorkflowStateChanged(unsigned int u1State, unsigned int u2State);
    void onMachineTool_NextCommandSent(QByteArray package);
    void on_pausePushButton_clicked();
    void on_resumePushButton_clicked();
};

#endif // PROGRAM_PROCESSING_DIALOG_H
