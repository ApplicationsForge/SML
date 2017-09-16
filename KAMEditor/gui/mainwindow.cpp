#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // окно на весь экран
    QMainWindow::showMaximized();

    // настройка станка
    setupMachineTool();

    // настройка виджетов
    setupWidgets();

    setupTimer();
}

MainWindow::~MainWindow()
{
    delete timer;
    delete ui;

    // удаляем горячие клавиши
    while (axisesShortcuts.size() > 0)
    {
        delete axisesShortcuts.back();
        axisesShortcuts.pop_back();
    }
    delete hightlighter;
    //delete u1Connector;

#ifdef Q_OS_WIN
    delete u1Manager;
#endif
    delete machineTool;

}

void MainWindow::setupWidgets()
{
    // задаем горячие клавиши
    setupAxisesShortcuts();

    // проводим настройку необходимых виджетов
    setupStatusBar();
    setupTreeWidget();
    setupGCodesSyntaxHighlighter();
    setupEdgesControl();
    setupPointsTableWidgets();
    setupPointsPushButtons();
    setupEditorFileActionsPushButtons();
    setupCoordinatesPanel();
}

void MainWindow::setupTreeWidget()
{
    QTreeWidget*  editorField = ui->smlEditorTreeWidget;
    // установка древесной структуры в 1 столбец виджета отображения sml-команд
    editorField->setTreePosition(1);

    // устанавливаем слоты, обрабатывающие сигналы дерева
    connect(editorField, SIGNAL(copySignal()), this, SLOT(commandsCopySlot()));
    connect(editorField, SIGNAL(cutSignal()), this, SLOT(commandsCutSlot()));
    connect(editorField, SIGNAL(pasteSignal()), this, SLOT(commandsPasteSlot()));
    connect(editorField, SIGNAL(undoSignal()), this, SLOT(commandsUndoSlot()));
    connect(editorField, SIGNAL(eraseSignal()), this, SLOT(deleteSelectedCommands()));
}

void MainWindow::setupStatusBar()
{
    // установка оформления statusBar
    ui->statusBar->setStyleSheet("background-color: #333; color: #33bb33");
    ui->statusBar->setFont(QFont("Consolas", 14));
    ui->statusBar->showMessage(tr("State: ready 0123456789"));
}

void MainWindow::setupGCodesSyntaxHighlighter()
{
    // устанвиливаем подсветку текста в виджете отображения G-кодов
    hightlighter = new GCodesSyntaxHighlighter(this);
    hightlighter->setDocument(ui->gcodesEditorTextEdit->document());
    hightlighter->setPattern();
}

void MainWindow::setupEdgesControl()
{
    // синхронизаци контроля габаритов и соответствующих элементов интерфейса
    updateEdgesControlStatus();
    connect(ui->edgesControlCheckBox, SIGNAL(clicked(bool)), this, SLOT(updateEdgesControlStatus()));
}

void MainWindow::setupPointsTableWidgets()
{
    std::vector< std::shared_ptr<Axis> > axises = machineTool->getMovementController()->getAxises();
    int axisesCount = axises.size();
    QStringList axisesLabels;

    for(auto axis : axises)
    {
        axisesLabels.push_back(QString::fromStdString(axis->getName()));
    }

    ui->pointsTableWidget->setColumnCount(axisesCount);
    ui->pointsTableWidget_2->setColumnCount(axisesCount);

    ui->pointsTableWidget->setHorizontalHeaderLabels(axisesLabels);
    ui->pointsTableWidget_2->setHorizontalHeaderLabels(axisesLabels);

    // растянуть таблицу с координатами редактора точек
    for (int i = 0; i < ui->pointsTableWidget->horizontalHeader()->count(); i++)
    {
        ui->pointsTableWidget->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
        ui->pointsTableWidget_2->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
    }

    // в таблице редактора точек выделяется целиком вся строка
    ui->pointsTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->pointsTableWidget_2->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->pointsTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->pointsTableWidget_2->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(ui->pointsTableWidget, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(on_pointEditPushButton_clicked()));
    connect(ui->pointsTableWidget_2, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(on_pointEditPushButton_clicked()));
}

void MainWindow::setupPointsPushButtons()
{
    connect(ui->pointAddPushButton_2, SIGNAL(clicked(bool)), this, SLOT(on_pointAddPushButton_clicked()));
    connect(ui->pointDeletePushButton_2, SIGNAL(clicked(bool)), this, SLOT(on_pointDeletePushButton_clicked()));
    connect(ui->pointCursorPushButton_2, SIGNAL(clicked(bool)), this, SLOT(on_pointCursorPushButton_clicked()));
    connect(ui->pointCopyPushButton_2, SIGNAL(clicked(bool)), this, SLOT(on_pointCopyPushButton_clicked()));

}

void MainWindow::setupMachineTool()
{
    SettingsManager settingsManager;
    QString machineToolInformationGroupName = "MachineToolInformation";

    try
    {
        machineTool = new MachineTool(
                    settingsManager.get(machineToolInformationGroupName, "VendorId").toUInt(),
                    settingsManager.get(machineToolInformationGroupName, "ProductId").toUInt(),
                    settingsManager.get(machineToolInformationGroupName, "Name").toString().toStdString(),
                    settingsManager.get(machineToolInformationGroupName, "AxisCount").toUInt()
                    );
    }
    catch(std::invalid_argument e)
    {
        QMessageBox(QMessageBox::Warning, "Ошибка инициализации", QString("Ошибка инициализации станка!") + QString(e.what()) + QString("; Приложение будет закрыто.")).exec();
        on_exit_action_triggered();
    }

    updateSettingsFields();
    updateSensorsPanel();
    updateDevicesPanel();


    unsigned int velocity = machineTool->getVelocity();
    ui->feedrateScrollBar->setValue(velocity);

    unsigned int spindelRotations = machineTool->getSpindelRotations();
    ui->rotationsScrollBar->setValue(spindelRotations);

#ifdef Q_OS_WIN
    try
    {
        u1Manager = new UsbXpressDeviceManager(machineTool->getName());
        showMachineToolConnected();
    }
    catch(std::runtime_error e)
    {
        u1Manager = nullptr;
        QMessageBox(QMessageBox::Warning, "Ошибка подключения", e.what()).exec();
        showMachineToolDisconnected();
    }
#endif

    QString machineToolId = "Vendor Id = " + QString::number(machineTool->getVendorId()) + "; Product Id = "+ QString::number(machineTool->getProductId());
    ui->machineToolsVIdPIdLineEdit->setText(machineToolId);
    ui->machineToolsComboBox->addItem(QString::fromStdString(machineTool->getName()));

    /*u1Connector = new UsbDevicesManager(machineTool);
    if(u1Connector->getU1() != NULL)
    {
        ui->statusBar->setStyleSheet("background-color: #333; color: #33bb33");
        ui->statusBar->showMessage("Machine Tool is connected");
    }
    else
    {
        ui->statusBar->setStyleSheet("background-color: #333; color: #b22222");
        ui->statusBar->showMessage("Machine Tool is disconected");
    }*/
}

void MainWindow::updateSettingsFields()
{
    updateAxisSettingsField();
    updateSensorsSettingsField();
    updateDevicesSettingsField();
}

void MainWindow::updateAxisSettingsField()
{
    ui->axisSettingsTableWidget->clear();

    std::vector< std::shared_ptr<Axis> > axises = machineTool->getMovementController()->getAxises();
    int axisCount = axises.size();

    QStringList qHorizontalHeaders;
    for(auto axis : axises)
    {
        QString header = QString("Ось " + QString::fromStdString(axis->getName()));
        qHorizontalHeaders.append(header);
    }

    ui->axisSettingsTableWidget->setColumnCount(axisCount);
    ui->axisSettingsTableWidget->setHorizontalHeaderLabels(qHorizontalHeaders);

    QStringList qVerticalHeaders =
    {
        "Длина",
        "Шаг",
        "Направление",
        "Рывок",
        "Ускорение",
        "Скорость",
        "Скорость Базирования",
        "Канал"
    };
    ui->axisSettingsTableWidget->setRowCount(qVerticalHeaders.size());
    ui->axisSettingsTableWidget->setVerticalHeaderLabels(qVerticalHeaders);


    for(int i = 0; i < ui->axisSettingsTableWidget->horizontalHeader()->count(); i++)
    {
        ui->axisSettingsTableWidget->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
        for(int j = 0; j < ui->axisSettingsTableWidget->verticalHeader()->count(); j++)
        {
            QTableWidgetItem *item = fillAxisesSettingsTable(axises, i, j);
            ui->axisSettingsTableWidget->setItem(j, i, item);
        }
    }

    for(int i = 0; i < ui->axisSettingsTableWidget->verticalHeader()->count(); i++)
    {
        ui->axisSettingsTableWidget->verticalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
    }
}

QTableWidgetItem* MainWindow::fillAxisesSettingsTable(const std::vector< std::shared_ptr<Axis> > &axises, int axisIndex, int parametrIndex)
{
    std::string text = "Здесь должны быть параметры оси";
    switch (parametrIndex) {
    case 0:
        text = std::to_string(axises[axisIndex]->getLength());
        break;
    case 1:
        text = std::to_string(axises[axisIndex]->getStep());
        break;
    case 2:
        text = std::to_string(axises[axisIndex]->getInvertDirection());
        break;
    case 3:
        text = std::to_string(axises[axisIndex]->getJerk());
        break;
    case 4:
        text = std::to_string(axises[axisIndex]->getAcceleration());
        break;
    case 5:
        text = std::to_string(axises[axisIndex]->getVelocity());
        break;
    case 6:
        text = std::to_string(axises[axisIndex]->getBasingVelocity());
        break;
    case 7:
        text = std::to_string(axises[axisIndex]->getChannel());
        break;
    default:
        text = "Unknown parametr";
        break;
    }
    return new QTableWidgetItem(QString::fromStdString(text));
}


void MainWindow::updateSensorsSettingsField()
{
    std::vector< std::shared_ptr<Sensor> > sensors = machineTool->getSensorsManager()->getSensors();
    int sensorsCount = sensors.size();
    QStringList sensorsLabels;
    for(auto sensor : sensors)
    {
        sensorsLabels.push_back(QString::fromStdString(sensor->getName()));
    }
    ui->sensorsSettingsTableWidget->setRowCount(sensorsCount);
    ui->sensorsSettingsTableWidget->setVerticalHeaderLabels(sensorsLabels);


    QStringList qHorizontalHeaders =
    {
        "Имя платы",
        "Номер порта",
        "Номер входа",
        "Активное состояние",
    };
    ui->sensorsSettingsTableWidget->setColumnCount(qHorizontalHeaders.size());
    ui->sensorsSettingsTableWidget->setHorizontalHeaderLabels(qHorizontalHeaders);

    for (int i = 0; i < ui->sensorsSettingsTableWidget->horizontalHeader()->count(); i++)
    {
        ui->sensorsSettingsTableWidget->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
        for(int j = 0; j < ui->sensorsSettingsTableWidget->verticalHeader()->count(); j++)
        {
            QTableWidgetItem *item = fillSensorsSettingsTable(sensors, i, j);
            ui->sensorsSettingsTableWidget->setItem(j, i, item);
        }
    }
}

QTableWidgetItem* MainWindow::fillSensorsSettingsTable(const std::vector< std::shared_ptr<Sensor> > &sensors, int parametrIndex, int sensorIndex)
{
    std::string text = "Здесь должны быть параметры Датчика";
    switch (parametrIndex) {
    case 0:
        text = sensors[sensorIndex]->getBoardName();
        break;
    case 1:
        text = std::to_string(sensors[sensorIndex]->getPortNumber());
        break;
    case 2:
        text = std::to_string(sensors[sensorIndex]->getInputNumber());
        break;
    case 3:
        text = std::to_string(sensors[sensorIndex]->getActiveState());
        break;
    default:
        text = "Unknown parametr";
        break;
    }
    return new QTableWidgetItem(QString::fromStdString(text));
}

void MainWindow::updateDevicesSettingsField()
{
    std::vector< std::shared_ptr<Device> > devices = machineTool->getDevicesManager()->getDevices();
    int devicesCount = devices.size();
    QStringList devicesLabels;
    for(auto device : devices)
    {
        devicesLabels.push_back(QString::fromStdString(device->getName()));
    }
    ui->devicesSettingsTableWidget->setRowCount(devicesCount);
    ui->devicesSettingsTableWidget->setVerticalHeaderLabels(devicesLabels);


    QStringList qHorizontalHeaders =
    {
        "Имя платы",
        "Номер порта",
        "Номер выхода",
        "Активное состояние",
        "Маска"
    };
    ui->devicesSettingsTableWidget->setColumnCount(qHorizontalHeaders.size());
    ui->devicesSettingsTableWidget->setHorizontalHeaderLabels(qHorizontalHeaders);

    // растянуть таблицу с координатами
    for (int i = 0; i < ui->devicesSettingsTableWidget->horizontalHeader()->count(); i++)
    {
        ui->devicesSettingsTableWidget->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
        for(int j = 0; j < ui->devicesSettingsTableWidget->verticalHeader()->count(); j++)
        {
            QTableWidgetItem *item = fillDevicesSettingsTable(devices, i, j);
            ui->devicesSettingsTableWidget->setItem(j, i, item);
        }
    }
}

QTableWidgetItem* MainWindow::fillDevicesSettingsTable(const std::vector<std::shared_ptr<Device> > &devices, int parametrIndex, int deviceIndex)
{
    QString text = "Здесь должны быть параметры Устройства";
    switch (parametrIndex) {
    case 0:
        text = QString::fromStdString(devices[deviceIndex]->getBoardName());
        break;
    case 1:
        text = QString::number(devices[deviceIndex]->getPortNumber());
        break;
    case 2:
        text = QString::number(devices[deviceIndex]->getOutputNumber());
        break;
    case 3:
        text = QString::number(devices[deviceIndex]->getActiveState());
        break;
    case 4:
        text = QString::number(devices[deviceIndex]->getMask(), 2);
        break;
    default:
        text = "Unknown parametr";
        break;
    }
    return new QTableWidgetItem(text);
}

void MainWindow::setupCoordinatesPanel()
{
    QStringList axisesLabels;
    std::vector< std::shared_ptr<Axis> > axises = machineTool->getMovementController()->getAxises();
    for(auto axis : axises)
    {
        axisesLabels.push_back(QString(QString::fromStdString(axis->getName()) + ": "));
    }
    ui->currentCoordinatesListWidget->clear();
    ui->currentCoordinatesListWidget->addItems(axisesLabels);

    ui->baseCoordinatesListWidget->clear();
    ui->baseCoordinatesListWidget->addItems(axisesLabels);

    ui->parkCoordinatesListWidget->clear();
    ui->parkCoordinatesListWidget->addItems(axisesLabels);
}

void MainWindow::setupTimer()
{
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->setInterval(100);
    timer->start();
}

void MainWindow::updateSensorsPanel()
{
    ui->sensorsTableWidget->clear();
    std::vector< std::shared_ptr<Sensor> > sensors = machineTool->getSensorsManager()->getSensors();
    int sensorsCount = sensors.size();
    QStringList sensorsLabels;
    for(auto sensor : sensors)
    {
        sensorsLabels.push_back(QString::fromStdString(sensor->getName()));
    }
    ui->sensorsTableWidget->setRowCount(sensorsCount);
    ui->sensorsTableWidget->setVerticalHeaderLabels(sensorsLabels);
    ui->sensorsTableWidget->setColumnCount(1);

    for(int i = 0; i < ui->sensorsTableWidget->verticalHeader()->count(); i++)
    {
        bool isEnable = sensors[i]->isActive();
        QTableWidgetItem *item = new QTableWidgetItem();
        if(!isEnable)
        {
            item->setBackgroundColor(QColor(SmlColors::white()));
        }
        else
        {
            item->setBackgroundColor(sensors[i]->getColor());
        }
        ui->sensorsTableWidget->setItem(i, 0, item);
    }
    for(int i = 0; i < ui->sensorsTableWidget->verticalHeader()->count(); i++)
    {
        ui->sensorsTableWidget->verticalHeader()->setSectionResizeMode(i, QHeaderView::Fixed);
    }
}

void MainWindow::updateDevicesPanel()
{
    ui->devicesTableWidget->clear();
    std::vector< std::shared_ptr<Device> > devices = machineTool->getDevicesManager()->getDevices();
    ui->devicesTableWidget->setColumnCount(1);

    std::vector<QTableWidgetItem*> items;
    for(unsigned int i = 0; i < devices.size(); i++)
    {
        if(devices[i]->getNeedToDisplay())
        {
            QTableWidgetItem* item = new QTableWidgetItem();
            item->setText(QString::fromStdString(devices[i]->getName()));
            if(devices[i]->isEnable())
            {
                item->setTextColor(QColor(SmlColors::white()));
                item->setBackgroundColor(QColor(SmlColors::red()));
            }
            else
            {
                item->setTextColor(QColor(SmlColors::gray()));
                item->setBackgroundColor(QColor(SmlColors::white()));
            }
            items.push_back(item);
        }
    }
    ui->devicesTableWidget->setRowCount(items.size());
    for(int i = 0; i < ui->devicesTableWidget->rowCount(); i++)
    {
        ui->devicesTableWidget->setItem(i, 0, items[i]);
    }
    ui->devicesTableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
}

void MainWindow::setupAxisesShortcuts()
{
    std::vector<std::tuple <const char*, QPushButton*, const char*> > shortcutsMap = {
        std::make_tuple("A", ui->movementXNegativePushButton, SLOT(on_movementXNegativePushButton_clicked())),
        std::make_tuple("D", ui->movementXPositivePushButton, SLOT(on_movementXPositivePushButton_clicked())),
        std::make_tuple("S", ui->movementYNegativePushButton, SLOT(on_movementYNegativePushButton_clicked())),
        std::make_tuple("W", ui->movementYPositivePushButton, SLOT(on_movementYPositivePushButton_clicked())),
        std::make_tuple("Z", ui->movementXNegativeYNegativePushButton, SLOT(on_movementXNegativeYNegativePushButton_clicked())),
        std::make_tuple("Q", ui->movementXNegativeYPositivePushButton, SLOT(on_movementXNegativeYPositivePushButton_clicked())),
        std::make_tuple("X", ui->movementXPositiveYNegativePushButton, SLOT(on_movementXPositiveYNegativePushButton_clicked())),
        std::make_tuple("E", ui->movementXPositiveYPositivePushButton, SLOT(on_movementXPositiveYPositivePushButton_clicked())),
        std::make_tuple("Down", ui->movementZNegativePushButton, SLOT(on_movementZNegativePushButton_clicked())),
        std::make_tuple("Up", ui->movementZPositivePushButton, SLOT(on_movementZPositivePushButton_clicked())),
        std::make_tuple("Left", ui->movementANegativePushButton, SLOT(on_movementANegativePushButton_clicked())),
        std::make_tuple("Right", ui->movementAPositivePushButton, SLOT(on_movementAPositivePushButton_clicked())),
    };

    for (auto i = shortcutsMap.begin(); i != shortcutsMap.end(); i++)
    {
        const char* shortcutKey = std::get<0>(*i);
        QPushButton* shortcutButton = std::get<1>(*i);
        const char* shortcutSlot = std::get<2>(*i);

        QShortcut* shortcut = new QShortcut(QKeySequence(shortcutKey), shortcutButton);
        connect(shortcut, SIGNAL(activated()), this, shortcutSlot);

        axisesShortcuts.push_back(shortcut);
    }
}

void MainWindow::setupEditorFileActionsPushButtons()
{
    connect(ui->openFilePushButton, SIGNAL(clicked()), this, SLOT(on_open_action_triggered()));
}

void MainWindow::update()
{
    updateBatteryStatusPanel();
#ifdef Q_OS_WIN
    if(u1Manager != nullptr)
    {
        updateMachineToolStatus();
        updateCoordinates();
        //updateKabriolAvaliability();
        //updateBaseStatus();
    }
#endif
}

void MainWindow::deleteSelectedCommands()
{
    if(ui->editorTab->isVisible())
    {
        QList<QTreeWidgetItem*> selectedCommandsItems = ui->smlEditorTreeWidget->selectedItems();
        if(selectedCommandsItems.size() > 0)
        {
            std::vector<unsigned int> selectedCommandsIndexes;
            for(auto item : selectedCommandsItems)
            {
                selectedCommandsIndexes.push_back(item->text(0).toUInt() - 1);
            }
            std::sort(selectedCommandsIndexes.begin(), selectedCommandsIndexes.begin() + selectedCommandsIndexes.size());
            std::reverse(selectedCommandsIndexes.begin(), selectedCommandsIndexes.begin() + selectedCommandsIndexes.size());
            try
            {
                for(auto commandIndex : selectedCommandsIndexes)
                {
                    machineTool->getCommandsManager()->deleteCommand(commandIndex);
                }
                updateSMLCommandsTreeWidget();
            }
            catch(std::out_of_range e)
            {
                QMessageBox(QMessageBox::Warning, "Ошибка", e.what()).exec();
            }
        }
    }
}

void MainWindow::updateCoordinatesPanel()
{
    /*MachineTool &i = MachineTool::Instance();

    VectorDouble current = i.getCurrentCoordinates();
    VectorDouble base = i.getBaseCoordinates();
    VectorDouble park = i.getParkCoordinates();

    std::vector<std::pair<QListWidget*, VectorDouble>> widgets = {
        { ui->currentCoordinatesListWidget, current },
        { ui->baseCoordinatesListWidget, base },
        { ui->parkCoordinatesListWidget, park }
    };

    for (auto i = widgets.begin(); i != widgets.end(); i++)
    {
        i->first->item(0)->setText("X: " + QString::number(i->second.x, 'f', 3) + " мм");
        i->first->item(1)->setText("Y: " + QString::number(i->second.y, 'f', 3) + " мм");
        i->first->item(2)->setText("Z: " + QString::number(i->second.z, 'f', 3) + " мм");
        i->first->item(3)->setText("A: " + QString::number(i->second.a, 'f', 3) + " град");
        i->first->item(4)->setText("B: " + QString::number(i->second.b, 'f', 3) + " град");
    }*/
}

void MainWindow::updatePointsEditorTableWidgets()
{
    PointsManager pointsManager = *(machineTool->getPointsManager());
    unsigned int pointsCount = pointsManager.pointCount();
    std::vector<QTableWidget*> tables = { ui->pointsTableWidget, ui->pointsTableWidget_2 };
    // проходим по каждой таблице
    for (auto table = tables.begin(); table != tables.end(); table++)
    {
        // очищаем текущую таблицу
        (*table)->setRowCount(0);

        // проходим по всем точкам
        for (unsigned int i = 0; i < pointsCount; i++)
        {
            std::shared_ptr<Point> p = pointsManager[i];
            // добавляем строку в таблицу для текущей точки
            (*table)->insertRow(i);

            // отображаем координаты текущей точки
            for (unsigned int coordinate = 0; coordinate < p->size(); coordinate++)
            {
                std::string argument;
                try
                {
                    argument = std::to_string(p.get()->operator [](coordinate));
                }
                catch(std::out_of_range e)
                {
                    QMessageBox(QMessageBox::Warning, "Ошибка", e.what()).exec();
                    break;
                }
                (*table)->setItem(i, coordinate, new QTableWidgetItem( QString::fromStdString(argument) ));
            }
        }
    }
}

void MainWindow::updateSMLCommandsTreeWidget()
{
    ui->smlEditorTreeWidget->clear();
    unsigned int commandsCount = machineTool->getCommandsManager()->commandsCount();
    QList<QTreeWidgetItem*> qSmlCommands;

    for(unsigned int i = 0; i < commandsCount; i++)
    {

        QStringList commandStringList =
        {
            QString::number(i+1),
            QString::fromStdString(machineTool->getCommandsManager()->operator [](i)->getName()),
            machineTool->getCommandsManager()->operator [](i)->getArgumentsString()
        };


        QTreeWidgetItem* item = new QTreeWidgetItem(commandStringList);
        for(int j = 1; j < ui->smlEditorTreeWidget->columnCount(); j++)
        {
            item->setTextColor(j, machineTool->getCommandsManager()->operator [](i)->getColor());
        }
        qSmlCommands.push_back(item);
    }
    ui->smlEditorTreeWidget->addTopLevelItems(qSmlCommands);

    for(int i = 0; i < ui->smlEditorTreeWidget->columnCount() - 1; i++)
    {
        ui->smlEditorTreeWidget->resizeColumnToContents(i);
    }
}

void MainWindow::updateBatteryStatusPanel()
{
    #ifdef Q_OS_WIN
        SYSTEM_POWER_STATUS status;
        GetSystemPowerStatus(&status);

        if ((status.BatteryLifePercent < 0) || (status.BatteryLifePercent > 100))
        status.BatteryLifePercent = 100;

        ui->batteryProgressBar->setValue(status.BatteryLifePercent);
    #endif
    #ifdef Q_OS_OSX
        ui->batteryProgressBar->setValue(100);
    #endif
}

void MainWindow::updateBaseStatus()
{

}

void MainWindow::updateMachineToolStatusPanel()
{
#ifdef Q_OS_WIN
    ui->recievedDataTextEdit->clear();
    try
    {
        byte_array recieved = u1Manager->getU1()->receiveData(16);
        machineTool->getBuffer().updateBuffer(recieved);

        machineTool->getSensorsManager()->updateSensors(machineTool->getBuffer());
        updateSensorsField();

        QString recievedData;
        for(auto it : recieved)
        {
            recievedData = QString::number(it, 2);
            ui->recievedDataTextEdit->append(recievedData);
        }
        showMachineToolConnected();
    }
    catch(std::runtime_error e)
    {
        QMessageBox(QMessageBox::Warning, "Ошибка", e.what()).exec();
        timer->stop();
        showMachineToolDisconnected();
    }
#endif
}

void MainWindow::showMachineToolConnected()
{
    ui->statusBar->setStyleSheet("background-color: #333; color: #33bb33");
    ui->statusBar->showMessage("Machine Tool is connected");
}

void MainWindow::showMachineToolDisconnected()
{
    ui->statusBar->setStyleSheet("background-color: #333; color: #b22222");
    ui->statusBar->showMessage("Machine Tool is disconected");
}

void MainWindow::disableMovementButtonsShortcuts()
{
    setMovementButtonsShortcutsState(false);
}

void MainWindow::enableMovementButtonsShortcuts()
{
    setMovementButtonsShortcutsState(true);
}

void MainWindow::setMovementButtonsShortcutsState(bool state)
{
    for (auto i = axisesShortcuts.begin(); i != axisesShortcuts.end(); i++)
        (*i)->setAutoRepeat(state);
}

void MainWindow::setMovementButtonsRepeatState(bool state)
{
    std::vector<QPushButton*> movementButtons = {
        ui->movementXPositivePushButton,
        ui->movementXNegativePushButton,
        ui->movementYPositivePushButton,
        ui->movementYNegativePushButton,

        ui->movementXPositiveYPositivePushButton,
        ui->movementXPositiveYNegativePushButton,
        ui->movementXNegativeYPositivePushButton,
        ui->movementXNegativeYNegativePushButton,

        ui->movementZPositivePushButton,
        ui->movementZNegativePushButton,

        ui->movementAPositivePushButton,
        ui->movementANegativePushButton
    };

    for (std::vector<QPushButton*>::iterator i = movementButtons.begin(); i != movementButtons.end(); i++)
        (*i)->setAutoRepeat(state);
}

void MainWindow::on_discreteRadioButton_1_clicked()
{
    //MachineTool::Instance().setMovementStep(0.01);

    disableMovementButtonsShortcuts();
    setMovementButtonsRepeatState(false);
}

void MainWindow::on_discreteRadioButton_2_clicked()
{
    //MachineTool::Instance().setMovementStep(0.1);

    disableMovementButtonsShortcuts();
    setMovementButtonsRepeatState(false);
}

void MainWindow::on_discreteRadioButton_3_clicked()
{
    //MachineTool::Instance().setMovementStep(1);

    disableMovementButtonsShortcuts();
    setMovementButtonsRepeatState(false);
}

void MainWindow::on_discreteRadioButton_4_clicked()
{
    //MachineTool::Instance().setMovementStep(10);

    disableMovementButtonsShortcuts();
    setMovementButtonsRepeatState(false);
}

void MainWindow::on_discreteRadioButton_5_clicked()
{
    //MachineTool::Instance().setMovementStep(0);

    enableMovementButtonsShortcuts();
    setMovementButtonsRepeatState(true);
}

void MainWindow::on_movementXPositivePushButton_clicked()
{
    /*MachineTool &i = MachineTool::Instance();
    VectorDouble v = VectorDouble();
    v.x = 1;

    i.stepMove(v);*/
}

void MainWindow::on_movementXNegativePushButton_clicked()
{
    /*MachineTool &i = MachineTool::Instance();
    VectorDouble v = VectorDouble();
    v.x = -1;

    i.stepMove(v);*/
}

void MainWindow::on_movementYPositivePushButton_clicked()
{
    /*MachineTool &i = MachineTool::Instance();
    VectorDouble v = VectorDouble() ;
    v.y = 1;

    i.stepMove(v);*/
}

void MainWindow::on_movementYNegativePushButton_clicked()
{
    /*MachineTool &i = MachineTool::Instance();
    VectorDouble v = VectorDouble() ;
    v.y = -1;

    i.stepMove(v);*/
}

void MainWindow::on_movementXNegativeYPositivePushButton_clicked()
{
    /*MachineTool &i = MachineTool::Instance();
    VectorDouble v = VectorDouble() ;
    v.x = -1;
    v.y = 1;

    i.stepMove(v);*/
}

void MainWindow::on_movementXPositiveYPositivePushButton_clicked()
{
    /*MachineTool &i = MachineTool::Instance();
    VectorDouble v = VectorDouble() ;
    v.x = 1;
    v.y = 1;

    i.stepMove(v);*/
}

void MainWindow::on_movementXNegativeYNegativePushButton_clicked()
{
    /*MachineTool &i = MachineTool::Instance();
    VectorDouble v = VectorDouble() ;
    v.x = -1;
    v.y = -1;

    i.stepMove(v);*/
}

void MainWindow::on_movementXPositiveYNegativePushButton_clicked()
{
    /*MachineTool &i = MachineTool::Instance();
    VectorDouble v = VectorDouble() ;
    v.x = 1;
    v.y = -1;

    i.stepMove(v);*/
}

void MainWindow::on_movementZPositivePushButton_clicked()
{
    /*MachineTool &i = MachineTool::Instance();
    VectorDouble v = VectorDouble() ;
    v.z = 1;

    i.stepMove(v);*/
}

void MainWindow::on_movementZNegativePushButton_clicked()
{
    /*MachineTool &i = MachineTool::Instance();
    VectorDouble v = VectorDouble() ;
    v.z = -1;

    i.stepMove(v);*/

}

void MainWindow::on_movementAPositivePushButton_clicked()
{
    /*MachineTool &i = MachineTool::Instance();
    VectorDouble v = VectorDouble() ;
    v.a = 1;

    i.stepMove(v);*/
}

void MainWindow::on_movementANegativePushButton_clicked()
{
    /*MachineTool &i = MachineTool::Instance();
    VectorDouble v = VectorDouble() ;
    v.a = -1;

    i.stepMove(v);*/
}

void MainWindow::on_feedrateScrollBar_valueChanged(int value)
{
    machineTool->setVelocity(value);
    ui->feedrateLcdNumber->display(QString::number(machineTool->getVelocity()));
}

void MainWindow::on_rotationsScrollBar_valueChanged(int value)
{
    machineTool->setSpindelRotations(value);
    ui->rotationsLcdNumber->display(QString::number(machineTool->getSpindelRotations()));
}

void MainWindow::on_exit_action_triggered()
{
    exit(0);
}

void MainWindow::on_pointsAmountPushButton_clicked()
{
    QMessageBox(QMessageBox::Information, "Количество точек", QString::number(ui->pointsTableWidget->rowCount())).exec();
}

void MainWindow::on_parkPushButton_clicked()
{

}

void MainWindow::on_zeroPushButton_clicked()
{

}

void MainWindow::on_pointAddPushButton_clicked()
{
    AddPointDialog* addPoint = new AddPointDialog(machineTool->getMovementController(), machineTool->getPointsManager(), this);
    addPoint->exec();
    updatePointsEditorTableWidgets();
}

void MainWindow::on_pointDeletePushButton_clicked()
{
    QList<QTableWidgetItem*> selected;
    std::set<int> rows;
    if(ui->adjustmentTab->isVisible())
    {
        selected = ui->pointsTableWidget->selectedItems();
        for (QList<QTableWidgetItem*>::iterator i = selected.begin(); i != selected.end(); i++)
        {
            int row = ui->pointsTableWidget->row(*i);
            rows.insert(row);
        }
    }
    else
    {
        if(ui->editorTab->isVisible())
        {
            selected = ui->pointsTableWidget_2->selectedItems();
            for (QList<QTableWidgetItem*>::iterator i = selected.begin(); i != selected.end(); i++)
            {
                int row = ui->pointsTableWidget_2->row(*i);
                rows.insert(row);
            }
        }
    }

    for (std::set<int>::reverse_iterator i = rows.rbegin(); i != rows.rend(); i++)
    {
        std::shared_ptr<Point> p = machineTool->getPointsManager()->operator [](*i);
        machineTool->getPointsManager()->deletePoint(p);
    }
    updatePointsEditorTableWidgets();
}

void MainWindow::on_pointCursorPushButton_clicked()
{
    ToSelectionPointDialog(machineTool->getMovementController(), machineTool->getPointsManager(), this).exec();
    updatePointsEditorTableWidgets();
}

void MainWindow::on_pointEditPushButton_clicked()
{
    QItemSelectionModel *select;
    if(ui->smlEditorTab->isVisible())
    {
        select = ui->pointsTableWidget_2->selectionModel();
    }
    else
    {
        select = ui->pointsTableWidget->selectionModel();
    }
    if(select->hasSelection())
    {
        //select->selectedRows();

        unsigned int current_row = (unsigned int) select->currentIndex().row();
        try
        {
            AddPointDialog* editPoint = new AddPointDialog(machineTool->getMovementController(), machineTool->getPointsManager(), machineTool->getPointsManager()->operator [](current_row), current_row, this);
            editPoint->exec();
            delete editPoint;
            updatePointsEditorTableWidgets();
        }
        catch(std::out_of_range e)
        {
            QMessageBox(QMessageBox::Warning, "Ошибка", e.what()).exec();
        }
    }
    else
    {
         QMessageBox(QMessageBox::Information, "Сообщение", QString("Точка не выбрана")).exec();
    }
}

void MainWindow::on_pointCopyPushButton_clicked()
{
    int selectedPointNumber = -1;
    if(ui->adjustmentTab->isVisible())
    {
        selectedPointNumber = ui->pointsTableWidget->currentRow();
    }
    else
    {
        if(ui->editorTab->isVisible())
        {
            selectedPointNumber = ui->pointsTableWidget_2->currentRow();
        }
    }

    if(selectedPointNumber >= 0)
    {
        try
        {
            std::shared_ptr<Point> currentPoint = machineTool->getPointsManager()->operator [](selectedPointNumber);
            Point* insertedPoint = new Point(*currentPoint.get());
            machineTool->getPointsManager()->addPoint(insertedPoint);
        }
        catch(std::out_of_range e)
        {
            QMessageBox(QMessageBox::Warning, "Ошибка", e.what()).exec();
        }
    }
    updatePointsEditorTableWidgets();
}

void MainWindow::updateEdgesControlStatus()
{
    if(ui->edgesControlCheckBox->isChecked())
    {
        ui->currentCoordinatesListWidget->setStyleSheet("border: 2px solid #2E8B57");
        ui->baseCoordinatesListWidget->setStyleSheet("border: 2px solid #2E8B57");
        ui->parkCoordinatesListWidget->setStyleSheet("border: 2px solid #2E8B57");
    }
    else
    {
        ui->currentCoordinatesListWidget->setStyleSheet("border: 2px solid #B22222");
        ui->baseCoordinatesListWidget->setStyleSheet("border: 2px solid #B22222");
        ui->parkCoordinatesListWidget->setStyleSheet("border: 2px solid #B22222");
    }
}

void MainWindow::on_toBasePushButton_clicked()
{

}

void MainWindow::on_open_action_triggered()
{
    QString path = QFileDialog::getOpenFileName(0, "Open Dialog", "", "*.txt, *.7kam");
    QFile inputFile(path);
    if(!inputFile.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(0, "error", inputFile.errorString());
    }
    QTextStream in(&inputFile);
    QString content = in.readAll();
    inputFile.close();

    ui->gcodesEditorTextEdit->setPlainText(content);
}


void MainWindow::on_gcodesEditorTextEdit_textChanged()
{
    QString text = ui->gcodesEditorTextEdit->toPlainText();
}

void MainWindow::on_importsettings_action_triggered()
{

}

void MainWindow::on_savesettings_action_triggered()
{

}

void MainWindow::on_startDegbugCommandLinkButton_clicked()
{
    //KM_New(NULL, 1);
    //KM_Free(NULL);
}

void MainWindow::on_finishDebugCommandLinkButton_clicked()
{
    ui->startDegbugCommandLinkButton->setEnabled(true);
    ui->finishDebugCommandLinkButton->setEnabled(false);
#ifdef Q_OS_WIN
    byte_array data =
    {
        16,
        0xff
    };
    try
    {
        u1Manager->getU1()->sendData(data);
    }
    catch(std::runtime_error e)
    {
        QMessageBox(QMessageBox::Warning, "Ошибка", e.what()).exec();
        showMachineToolDisconnected();
    }
#endif
}

void MainWindow::on_devicesTableWidget_clicked(const QModelIndex &index)
{
    std::string deviceName = index.data().toString().toStdString();
    try
    {
        Device &device = machineTool->getDevicesManager()->findDevice(deviceName);
        byte_array data = machineTool->getDevicesManager()->getSwitchDeviceData(device, !device.getCurrentState());
#ifdef Q_OS_WIN
        if(u1Manager != nullptr)
        {
            try
            {
                u1Manager->getU1()->sendData(data);
            }
            catch(std::runtime_error e)
            {
                QMessageBox(QMessageBox::Warning, "Ошибка", e.what()).exec();
            }
        }
        else
        {
            QMessageBox(QMessageBox::Warning, "Ошибка инициализации", "Не могу связаться со станком").exec();
        }
#endif
        device.setCurrentState(!device.getCurrentState());
        updateDevicesPanel();
    }
    catch(std::invalid_argument e)
    {
        QMessageBox(QMessageBox::Warning, "Ошибка", e.what()).exec();
    }
}

void MainWindow::on_commandsToolsListWidget_itemClicked(QListWidgetItem *item)
{
    QString commandName = item->text();
    int commandNumber = CommandsIds.getId(commandName.toStdString());

    size_t currentCommandNumber = machineTool->getCommandsManager()->commandsCount();

    QList<QTreeWidgetItem*> selectedItems = ui->smlEditorTreeWidget->selectedItems();
    if(selectedItems.size() > 0)
    {
        currentCommandNumber = selectedItems[0]->text(0).toInt()-1;
    }

    switch (commandNumber) {
    case CMD_SWITCH_ON:
        OnDialog(machineTool->getDevicesManager(), machineTool->getCommandsManager(), currentCommandNumber, this).exec();
        break;
    case CMD_SWITCH_OFF:
        OffDialog(machineTool->getDevicesManager(), machineTool->getCommandsManager(), currentCommandNumber, this).exec();
        break;
    case CMD_COMMENT:
        CommentDialog(machineTool->getCommandsManager(), currentCommandNumber, this).exec();
        break;
    case CMD_PAUSE:
        PauseDialog(machineTool->getCommandsManager(), currentCommandNumber, this).exec();
        break;
    case CMD_LINE:
        LineDialog(machineTool->getCommandsManager(), currentCommandNumber, this).exec();
        break;
    case CMD_ARC:
        ArcDialog(machineTool->getCommandsManager(), currentCommandNumber, this).exec();
        break;
    case CMD_TTLINE:
        TTLineDialog(machineTool->getCommandsManager(), machineTool->getPointsManager(), currentCommandNumber, this).exec();
        break;
    default:
        QMessageBox(QMessageBox::Warning, "Ошибка", "Неизвестная команда").exec();
        break;
    }
    updateSMLCommandsTreeWidget();
}

void MainWindow::on_viewPushButton_clicked()
{
    machineTool->getCommandsInterpreter()->updateProgram();
    ProgramVisualizeWindow(machineTool->getCommandsInterpreter(), machineTool->getPointsManager(), this).exec();
}

void MainWindow::on_smlEditorTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    if(column != 1)
    {
        column = 1;
    }
    QString commandName = item->text(column);
    int commandNumber = CommandsIds.getId(commandName.toStdString());

    unsigned int currentCommandNumber = item->text(0).toUInt() - 1;
    switch (commandNumber) {
    case CMD_SWITCH_ON:
        OnDialog(machineTool->getDevicesManager(), machineTool->getCommandsManager(), currentCommandNumber, this, true).exec();
        break;
    case CMD_SWITCH_OFF:
        OffDialog(machineTool->getDevicesManager(), machineTool->getCommandsManager(), currentCommandNumber, this, true).exec();
        break;
    case CMD_COMMENT:
        CommentDialog(machineTool->getCommandsManager(), currentCommandNumber, this, true).exec();
        break;
    case CMD_PAUSE:
        PauseDialog(machineTool->getCommandsManager(), currentCommandNumber, this, true).exec();
        break;
    case CMD_LINE:
        LineDialog(machineTool->getCommandsManager(), currentCommandNumber, this, true).exec();
        break;
    case CMD_ARC:
        ArcDialog(machineTool->getCommandsManager(), currentCommandNumber, this, true).exec();
        break;
    case CMD_TTLINE:
        TTLineDialog(machineTool->getCommandsManager(), machineTool->getPointsManager(), currentCommandNumber, this, true).exec();
        break;
    default:
        QMessageBox(QMessageBox::Warning, "Ошибка", "Выбранная команда не может быть отредактирована").exec();
        break;
    }
    updateSMLCommandsTreeWidget();
}

void MainWindow::on_kabriolWidgetPushButton_clicked()
{
    KabriolWindow(this).exec();
}

void MainWindow::on_toolLengthSensorPushButton_clicked()
{
    ToolLengthSensorWindow(this).exec();
}

void MainWindow::on_lubricationSystemPushButton_clicked()
{
    LubricationSystemWindow(machineTool->getDevicesManager(), this).exec();
}

void MainWindow::commandsCopySlot()
{
    qDebug() << "Copy signal received";
}

void MainWindow::commandsCutSlot()
{
    qDebug() << "Cut signal received";
}

void MainWindow::commandsPasteSlot()
{
    qDebug() << "Paste signal received";
}

void MainWindow::commandsUndoSlot()
{
    qDebug() << "Undo signal received";
}
