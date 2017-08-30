#ifndef LUBRICATIONSYSTEMWINDOW_H
#define LUBRICATIONSYSTEMWINDOW_H

#include <QDialog>
#include <QTimer>

#include "models/machinetool/devicesmanager/devicesmanager.h"

namespace Ui {
class LubricationSystemWindow;
}

class LubricationSystemWindow : public QDialog
{
    Q_OBJECT

public:
    explicit LubricationSystemWindow(DevicesManager* _devicesManager, QWidget *parent = 0);
    ~LubricationSystemWindow();

private slots:
    void on_lubricatePushButton_clicked();
    void stop();
    void updateProgressBar();

private:
    Ui::LubricationSystemWindow *ui;

    DevicesManager* devicesManager;

    QTimer* generalLubricationTimer;
    QTimer* stepLubriactionTimer;

    void switchLubricationSystem();
};

#endif // LUBRICATIONSYSTEMWINDOW_H
