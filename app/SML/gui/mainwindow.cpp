#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_connections(QList<QMetaObject::Connection>()),
    m_hightlighter(new GCodesSyntaxHighlighter(this))
{
    ui->setupUi(this);

    // окно на весь экран
    // QMainWindow::showMaximized();

    this->setupWidgets();
    this->hideWidgets();
    this->setupConnections();

    //updateAxisesBoard();
    //updateDevicesBoard();
    //updateSensorsBoard();

    //updatePointsEditorFields();
    //updatePointsEditorButtons();

    //updateVelocityPanel();
    //updateSpindelRotationsPanel();
    //updateOptionsPanel();
    //updateDevicesPanel();
    //updateServerPanel();

    MachineTool& machineTool = MachineTool::getInstance();
    this->onMachineTool_ErrorStateChanged(machineTool.getCurrentErrorFlags());
    this->on_discreteRadioButton_5_clicked();
}

MainWindow::~MainWindow()
{
    this->resetConnections();

    // удаляем горячие клавиши
    while (m_axisesShortcuts.size() > 0)
    {
        delete m_axisesShortcuts.back();
        m_axisesShortcuts.pop_back();
    }

    delete ui;
}

void MainWindow::setupWidgets()
{
    // изменяет TitleBar для DockWidget
    ui->dashboardDockWidget->setTitleBarWidget(new QWidget(ui->dashboardDockWidget));

    // установка оформления statusBar
    ui->statusBar->setStyleSheet("background-color: #333; color: #33bb33");
    ui->statusBar->setFont(QFont("Consolas", 14));
    ui->statusBar->showMessage(tr("State: ready 0123456789"));

    this->setupSensorsDisplay();
    this->setupSensorsSettingsBoard();

    this->setupSpindelsSettingsBoard();
    this->setupSupportDevicesSettingsBoard();
    this->setupSpindelsControlPanel();
    this->setupSupportDevicesControlPanel();

    this->setupCoordinatesDisplays();
    this->setupAxisesSettingsBoard();
    this->setupAxisesButtons();

    this->setupOptionsPanel();

    // настройка контроля габаритов
    //updateEdgesControlStatus();

    // настройка дисплеев координат
    /*QStringList axisesLabels;
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
    ui->parkCoordinatesListWidget->addItems(axisesLabels);*/

    // настройка редактора точек
    ui->pointsTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->pointsTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void MainWindow::setupConnections()
{
    MachineTool& machineTool = MachineTool::getInstance();

    m_connections.append(QObject::connect(&machineTool, SIGNAL(errorStateChanged(QList<ERROR_CODE>)), this, SLOT(onMachineTool_ErrorStateChanged(QList<ERROR_CODE>))));

    m_connections.append(QObject::connect(&machineTool, SIGNAL(pointsUpdated()), this, SLOT(onPointsUpdated())));
    m_connections.append(QObject::connect(&machineTool, SIGNAL(sensorStateChanged(QString,bool)), this, SLOT(onMachineTool_SensorStateChanged(QString,bool))));
    m_connections.append(QObject::connect(&machineTool, SIGNAL(spindelStateChanged(QString,bool,size_t)), this, SLOT(onMachineTool_SpindelStateChanged(QString,bool,size_t))));
    m_connections.append(QObject::connect(&machineTool, SIGNAL(gcodesFilePathUpdated(QString)), this, SLOT(onMachineTool_GCodesFilePathUpdated(QString))));
    m_connections.append(QObject::connect(&machineTool, SIGNAL(gcodesFileContentUpdated(QStringList)), this, SLOT(onMachineTool_GCodesFileContentUpdated(QStringList))));
    m_connections.append(QObject::connect(&machineTool, SIGNAL(taskCompletedSuccesfully()), this, SLOT(onMachineTool_TaskCompletedSuccesfully())));
    m_connections.append(QObject::connect(&machineTool, SIGNAL(taskCompletedWithErrors()), this, SLOT(onMachineTool_TaskCompletedWithErrors())));
    m_connections.append(QObject::connect(&machineTool, SIGNAL(currentCoordinatesChanged()), this, SLOT(onMachineTool_CurrentCoordinatesChanged())));
    m_connections.append(QObject::connect(&machineTool, SIGNAL(basingStateChanged(bool)), this, SLOT(onMachineTool_BasingStateChanged(bool))));

    /*QObject::connect(m_machineTool.data(), SIGNAL(u1StateIsChanged()), this, SLOT(updateU1Displays()));

    QObject::connect(m_machineTool.data(), SIGNAL(u2Connected()), this, SLOT(onU2Connected()));
    QObject::connect(m_machineTool.data(), SIGNAL(u2Disconnected()), this, SLOT(onU2Disconnected()));
    QObject::connect(m_machineTool.data(), SIGNAL(u2StateIsChanged()), this, SLOT(updateU1Displays()));

    QObject::connect(m_machineTool.data(), SIGNAL(u1Connected()), this, SLOT(updateServerPanel()));
    QObject::connect(m_machineTool.data(), SIGNAL(u1Disconnected()), this, SLOT(updateServerPanel()));
    QObject::connect(m_machineTool.data(), SIGNAL(u2Connected()), this, SLOT(updateServerPanel()));
    QObject::connect(m_machineTool.data(), SIGNAL(u2Disconnected()), this, SLOT(updateServerPanel()));

    QObject::connect(m_machineTool.data(), SIGNAL(machineToolErrorIsOccured(int)), this, SLOT(onMachineToolError(int)));

    QObject::connect(m_machineTool.data(), SIGNAL(gcodesUpdated()), this, SLOT(updateGCodesEditorWidget()));
    QObject::connect(m_machineTool.data(), SIGNAL(filePathUpdated()), this, SLOT(updateFilePath()));
    QObject::connect(m_machineTool.data(), SIGNAL(pointsUpdated()), this, SLOT(updatePointsEditorWidgets()));

    QObject::connect(ui->edgesControlCheckBox, SIGNAL(clicked(bool)), this, SLOT(updateEdgesControlStatus()));*/

    // задаем горячие клавиши
    for (auto i = m_shortcutsMap.begin(); i != m_shortcutsMap.end(); i++)
    {
        const char* shortcutKey = std::get<0>(*i);
        QPushButton* shortcutButton = std::get<1>(*i);
        const char* shortcutSlot = std::get<2>(*i);

        QShortcut* shortcut = new QShortcut(QKeySequence(shortcutKey), shortcutButton);
        m_connections.append(QObject::connect(shortcut, SIGNAL(activated()), this, shortcutSlot));

        m_axisesShortcuts.push_back(shortcut);
    }

    QList<PointsTableWidget*> pointsEditorTableWidgets = { ui->pointsTableWidget };
    for(auto pointsEditorTableWidget : pointsEditorTableWidgets)
    {
        m_connections.append(QObject::connect(pointsEditorTableWidget, SIGNAL(editSignal(QModelIndex)), this, SLOT(editPoint(QModelIndex))));
        m_connections.append(QObject::connect(pointsEditorTableWidget, SIGNAL(eraseSignal(QModelIndexList)), this, SLOT(deletePoints(QModelIndexList))));
        m_connections.append(QObject::connect(pointsEditorTableWidget, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(editPoint(QModelIndex))));
    }

    for(size_t i = 0; i < static_cast<size_t>(ui->spindelsListWidget->count()); i++)
    {
        SpindelControlWidget* widget = qobject_cast<SpindelControlWidget*> (ui->spindelsListWidget->itemWidget(ui->spindelsListWidget->item(static_cast<int>(i))));
        m_connections.append(QObject::connect(widget, SIGNAL(switchOn(QString,size_t)), &machineTool, SLOT(switchSpindelOn(QString,size_t))));
        m_connections.append(QObject::connect(widget, SIGNAL(switchOff(QString)), &machineTool, SLOT(switchSpindelOff(QString))));
    }

    // настройка импорта и экспорта настроек
    m_connections.append(QObject::connect(ui->importSettingsPushButton, SIGNAL(clicked(bool)), this, SLOT(on_importsettings_action_triggered())));
    m_connections.append(QObject::connect(ui->exportSettingsPushButton, SIGNAL(clicked(bool)), this, SLOT(on_savesettings_action_triggered())));

    // настройка кнопок работы с файлами
    m_connections.append(QObject::connect(ui->newFileToolButton, SIGNAL(clicked(bool)), this, SLOT(on_create_action_triggered())));
    m_connections.append(QObject::connect(ui->openFileToolButton, SIGNAL(clicked(bool)), this, SLOT(on_open_action_triggered())));
    m_connections.append(QObject::connect(ui->saveFileToolButton, SIGNAL(clicked(bool)), this, SLOT(on_save_action_triggered())));
    m_connections.append(QObject::connect(ui->saveFileAsToolButton, SIGNAL(clicked(bool)), this, SLOT(on_saveas_action_triggered())));
    m_connections.append(QObject::connect(ui->addFileToolButton, SIGNAL(clicked(bool)), this, SLOT(on_add_action_triggered())));
    m_connections.append(QObject::connect(ui->viewToolButton, SIGNAL(clicked(bool)), this, SLOT(on_view_action_triggered())));

    m_connections.append(QObject::connect(new QShortcut(QKeySequence("Esc"), this), &QShortcut::activated, this, [=]() {
        switch (ui->mainTabMenu->currentIndex()) {
        case 0:
            ui->mainTabMenu->setCurrentIndex(1);
            break;
        case 1:
            ui->mainTabMenu->setCurrentIndex(0);
            break;
        default:
            ui->mainTabMenu->setCurrentIndex(1);
            break;
        }
    }));
}

void MainWindow::resetConnections()
{
    for(auto& connection : m_connections)
    {
        QObject::disconnect(connection);
    }
}

void MainWindow::setupSensorsDisplay()
{
    MachineTool& machineTool = MachineTool::getInstance();

    QStringList names = machineTool.getRepository().getAllSensorsUids();

    for(auto name : names)
    {
        QMap<QString, QString> parameters = machineTool.getRepository().getSensorSettings(name);
        ui->sensorsDisplayWidget->addSensor(
                    parameters["Uid"],
                    parameters["Label"],
                    parameters["ActiveState"].toInt(),
                    parameters["LedColor"]);
    }
}

void MainWindow::onMachineTool_SensorStateChanged(QString name, bool state)
{
    ui->sensorsDisplayWidget->updateSensorState(name, state);
}

void MainWindow::onMachineTool_SpindelStateChanged(QString index, bool enable, size_t rotations)
{
    bool ok = false;

    size_t spindelIndex = index.toUInt(&ok);

    if(ok)
    {
        SpindelControlWidget* widget = qobject_cast<SpindelControlWidget*> (ui->spindelsListWidget->itemWidget(ui->spindelsListWidget->item(static_cast<int>(spindelIndex))));
        widget->updateControls(enable, rotations);
    }
}

void MainWindow::onMachineTool_BasingStateChanged(bool state)
{
    if(state)
    {
        this->enableMotionWidgets();
    }
    else
    {
        this->disableMotionWidgets();
    }
}

void MainWindow::setupSensorsSettingsBoard()
{
    MachineTool& machineTool = MachineTool::getInstance();

    QStringList sensorsSettings = machineTool.getRepository().getAllSensorsSettings();

    QStringList labels;
    QList< QPair<int, int> > positions;
    QList<QTableWidgetItem*> items;

    // rows
    for(int i = 0; i < sensorsSettings.size(); i++)
    {
        QStringList sensorSettings = sensorsSettings.at(i).split(";");
        // columns
        for(int j = 0; j < sensorSettings.size(); j++)
        {
            QStringList pair = sensorSettings[j].split(":");
            //qDebug() << pair;
            if(pair.size() == 2)
            {
                if(!labels.contains(pair.at(0)))
                {
                    labels.push_back(pair.at(0));
                }

                QTableWidgetItem* item = new QTableWidgetItem(pair.at(1));
                items.push_back(item);
                positions.push_back(QPair<int, int>(i, j));
            }
        }
    }

    ui->sensorsSettingsTableWidget->setColumnCount(labels.size());
    ui->sensorsSettingsTableWidget->setHorizontalHeaderLabels(labels);

    ui->sensorsSettingsTableWidget->setRowCount(sensorsSettings.size());
    for(int i = 0; i < items.size(); i++)
    {
        ui->sensorsSettingsTableWidget->setItem(positions[i].first, positions[i].second, items[i]);
    }
    ui->sensorsSettingsTableWidget->resizeColumnsToContents();
}

void MainWindow::setupSpindelsControlPanel()
{
    MachineTool& machineTool = MachineTool::getInstance();

    ui->spindelsListWidget->clear();
    auto spindels = machineTool.getRepository().getSpindels();
    for(auto spindel : spindels)
    {
        SpindelControlWidget* widget = new SpindelControlWidget(spindel->getLabel(),
                                                                spindel->getSettingsUId(),
                                                                spindel->getUid(),
                                                                spindel->getUpperBound(),
                                                                spindel->getLowerBound(),
                                                                spindel->getCurrentRotations(),
                                                                spindel->isEnable(),
                                                                ui->spindelsListWidget);

        QListWidgetItem* item = new QListWidgetItem();
        item->setSizeHint(widget->minimumSizeHint());

        ui->spindelsListWidget->addItem(item);
        ui->spindelsListWidget->setItemWidget(item, widget);
    }
    ui->spindelsListWidget->setStyleSheet("QListWidget { background-color: transparent; }");
}

void MainWindow::setupSpindelsSettingsBoard()
{
    MachineTool& machineTool = MachineTool::getInstance();
    QStringList spindelsSettings = machineTool.getRepository().getAllSpindelsSettings();

    QStringList labels;
    QList< QPair<int, int> > positions;
    QList<QTableWidgetItem*> items;

    // rows
    for(int i = 0; i < spindelsSettings.size(); i++)
    {
        QStringList spindelSettings = spindelsSettings.at(i).split(";");
        // columns
        for(int j = 0; j < spindelSettings.size(); j++)
        {
            QStringList pair = spindelSettings[j].split(":");
            //qDebug() << pair;
            if(pair.size() == 2)
            {
                if(!labels.contains(pair.at(0)))
                {
                    labels.push_back(pair.at(0));
                }

                QTableWidgetItem* item = new QTableWidgetItem(pair.at(1));
                items.push_back(item);
                positions.push_back(QPair<int, int>(i, j));
            }
        }
    }

    ui->spindelsSettingsTableWidget->setColumnCount(labels.size());
    ui->spindelsSettingsTableWidget->setHorizontalHeaderLabels(labels);

    ui->spindelsSettingsTableWidget->setRowCount(spindelsSettings.size());
    for(int i = 0; i < items.size(); i++)
    {
        ui->spindelsSettingsTableWidget->setItem(positions[i].first, positions[i].second, items[i]);
    }
}

void MainWindow::setupSupportDevicesControlPanel()
{
    ui->supportDevicesListWidget->setStyleSheet("QListWidget { background-color: transparent; }");

    QStringList supportDevices = { "Муфта1", "Тормоз1", "Лазер", "Муфта2", "Муфта3", "Тормоз2", "Тормоз3" };
    for(auto supportDevice : supportDevices)
    {
        QPushButton* itemWidget = new QPushButton(supportDevice, ui->supportDevicesListWidget);
        QListWidgetItem* item = new QListWidgetItem();
        item->setSizeHint(itemWidget->sizeHint());
        ui->supportDevicesListWidget->addItem(item);
        ui->supportDevicesListWidget->setItemWidget(item, itemWidget);
    }
}

void MainWindow::setupSupportDevicesSettingsBoard()
{
    MachineTool& machineTool = MachineTool::getInstance();
    QStringList supportDevicesSettings = machineTool.getRepository().getAllSupportDeviceSettings();

    QStringList labels;
    QList< QPair<int, int> > positions;
    QList<QTableWidgetItem*> items;

    // rows
    for(int i = 0; i < supportDevicesSettings.size(); i++)
    {
        QStringList supportDeviceSettings = supportDevicesSettings.at(i).split(";");
        // columns
        for(int j = 0; j < supportDeviceSettings.size(); j++)
        {
            QStringList pair = supportDeviceSettings[j].split(":");
            //qDebug() << pair;
            if(pair.size() == 2)
            {
                if(!labels.contains(pair.at(0)))
                {
                    labels.push_back(pair.at(0));
                }

                QTableWidgetItem* item = new QTableWidgetItem(pair.at(1));
                items.push_back(item);
                positions.push_back(QPair<int, int>(i, j));
            }
        }
    }

    ui->supportDevicesSettingsTableWidget->setColumnCount(labels.size());
    ui->supportDevicesSettingsTableWidget->setHorizontalHeaderLabels(labels);

    ui->supportDevicesSettingsTableWidget->setRowCount(supportDevicesSettings.size());
    for(int i = 0; i < items.size(); i++)
    {
        ui->supportDevicesSettingsTableWidget->setItem(positions[i].first, positions[i].second, items[i]);
    }
}

void MainWindow::setupCoordinatesDisplays()
{
    this->updateCoordinatesDisplays();
}

void MainWindow::setupAxisesSettingsBoard()
{
    MachineTool& machineTool = MachineTool::getInstance();

    QStringList axisesSettings = machineTool.getRepository().getAxesSettings();
    ui->axisesSettingsListWidget->addItems(axisesSettings);
}

void MainWindow::setupAxisesButtons()
{
    MachineTool& machineTool = MachineTool::getInstance();

    bool axisXEnabled = machineTool.getRepository().axisExists("X");
    bool axisYEnabled = machineTool.getRepository().axisExists("Y");
    bool axisZEnabled = machineTool.getRepository().axisExists("Z");
    bool axisAEnabled = machineTool.getRepository().axisExists("A");
    bool axisBEnabled = machineTool.getRepository().axisExists("B");
    bool axisCEnabled = machineTool.getRepository().axisExists("C");
    bool axisUEnabled = machineTool.getRepository().axisExists("U");
    bool axisVEnabled = machineTool.getRepository().axisExists("V");
    bool axisWEnabled = machineTool.getRepository().axisExists("W");

    if(axisXEnabled)
    {
        m_shortcutsMap.push_back(std::make_tuple("A", ui->movementXNegativePushButton, SLOT(on_movementXNegativePushButton_clicked())));
        m_shortcutsMap.push_back(std::make_tuple("D", ui->movementXPositivePushButton, SLOT(on_movementXPositivePushButton_clicked())));
    }
    ui->movementXNegativePushButton->setEnabled(axisXEnabled);
    ui->movementXPositivePushButton->setEnabled(axisXEnabled);

    if(axisYEnabled)
    {
        m_shortcutsMap.push_back(std::make_tuple("S", ui->movementYNegativePushButton, SLOT(on_movementYNegativePushButton_clicked())));
        m_shortcutsMap.push_back(std::make_tuple("W", ui->movementYPositivePushButton, SLOT(on_movementYPositivePushButton_clicked())));
    }
    ui->movementYNegativePushButton->setEnabled(axisYEnabled);
    ui->movementYPositivePushButton->setEnabled(axisYEnabled);

    if(axisXEnabled && axisYEnabled)
    {
        m_shortcutsMap.push_back(std::make_tuple("Z", ui->movementXNegativeYNegativePushButton, SLOT(on_movementXNegativeYNegativePushButton_clicked())));
        m_shortcutsMap.push_back(std::make_tuple("Q", ui->movementXNegativeYPositivePushButton, SLOT(on_movementXNegativeYPositivePushButton_clicked())));
        m_shortcutsMap.push_back(std::make_tuple("X", ui->movementXPositiveYNegativePushButton, SLOT(on_movementXPositiveYNegativePushButton_clicked())));
        m_shortcutsMap.push_back(std::make_tuple("E", ui->movementXPositiveYPositivePushButton, SLOT(on_movementXPositiveYPositivePushButton_clicked())));
    }
    ui->movementXNegativeYNegativePushButton->setEnabled(axisXEnabled && axisYEnabled);
    ui->movementXNegativeYPositivePushButton->setEnabled(axisXEnabled && axisYEnabled);
    ui->movementXPositiveYNegativePushButton->setEnabled(axisXEnabled && axisYEnabled);
    ui->movementXPositiveYPositivePushButton->setEnabled(axisXEnabled && axisYEnabled);

    if(axisZEnabled)
    {
        m_shortcutsMap.push_back(std::make_tuple("Down", ui->movementZNegativePushButton, SLOT(on_movementZNegativePushButton_clicked())));
        m_shortcutsMap.push_back(std::make_tuple("Up", ui->movementZPositivePushButton, SLOT(on_movementZPositivePushButton_clicked())));
    }
    ui->movementZNegativePushButton->setEnabled(axisZEnabled);
    ui->movementZPositivePushButton->setEnabled(axisZEnabled);

    if(axisAEnabled)
    {
        m_shortcutsMap.push_back(std::make_tuple("Left", ui->movementANegativePushButton, SLOT(on_movementANegativePushButton_clicked())));
        m_shortcutsMap.push_back(std::make_tuple("Right", ui->movementAPositivePushButton, SLOT(on_movementAPositivePushButton_clicked())));
    }

    ui->movementANegativePushButton->setEnabled(axisAEnabled);
    ui->movementAPositivePushButton->setEnabled(axisAEnabled);

    ui->movementBNegativePushButton->setEnabled(axisBEnabled);
    ui->movementBPositivePushButton->setEnabled(axisBEnabled);

    ui->movementCNegativePushButton->setEnabled(axisCEnabled);
    ui->movementCPositivePushButton->setEnabled(axisCEnabled);

    ui->movementUNegativePushButton->setEnabled(axisUEnabled);
    ui->movementUPositivePushButton->setEnabled(axisUEnabled);

    ui->movementVNegativePushButton->setEnabled(axisVEnabled);
    ui->movementVPositivePushButton->setEnabled(axisVEnabled);

    ui->movementWNegativePushButton->setEnabled(axisWEnabled);
    ui->movementWPositivePushButton->setEnabled(axisWEnabled);

    for(auto shortcut : m_axisesShortcuts)
    {
        shortcut->setEnabled(true);
    }
}

void MainWindow::updateCoordinatesDisplays()
{
    MachineTool& machineTool = MachineTool::getInstance();

    QList<Point> currentCoordinates = machineTool.getRepository().getCurrentPositionDetaiedInfo();

    if(currentCoordinates.length() == 3)
    {
        Point currentCoordinatesFromZero(currentCoordinates[0]);
        Point currentCoordinatesFromBase(currentCoordinates[1]);
        Point parkCoordinates(currentCoordinates[2]);

        this->showCoordinates(ui->currentCoordinatesListWidget, currentCoordinatesFromZero);
        this->showCoordinates(ui->baseCoordinatesListWidget, currentCoordinatesFromBase);
        this->showCoordinates(ui->parkCoordinatesListWidget, parkCoordinates);
    }
}

void MainWindow::showCoordinates(QListWidget *display, Point p)
{
    display->clear();
    QList< QPair<QString, double> > coords = p.coords();
    for(auto coordsItem : coords)
    {
        QString axisLabel = coordsItem.first + QString(": ") + QString::number(coordsItem.second, 'f', 3);
        display->addItem(axisLabel);
    }
}

void MainWindow::onMachineTool_GCodesFileContentUpdated(QStringList data)
{
    ui->gcodesEditorPlainTextEdit->setPlainText(data.join('\n'));
}

void MainWindow::onMachineTool_GCodesFilePathUpdated(QString path)
{
    ui->filePathLineEdit->setText(path);
}

void MainWindow::onMachineTool_TaskCompletedSuccesfully()
{
    //QMessageBox(QMessageBox::Information, "Успешное завершение работ", "Задание успешно выполнено").exec();
}

void MainWindow::onMachineTool_TaskCompletedWithErrors()
{
    MachineTool& machineTool = MachineTool::getInstance();
    QList<ERROR_CODE> errorCodes = machineTool.getCurrentErrorFlags();
    QString errorCodesString = "";
    for(auto errorCode : errorCodes)
    {
        errorCodesString += " #" + QString::number(errorCode);
    }

    QMessageBox(QMessageBox::Information, "Ошибка", "Выполнение задания завершено с ошибкой. Код ошибки = " + errorCodesString).exec();
}

void MainWindow::onMachineTool_CurrentCoordinatesChanged()
{
    this->updateCoordinatesDisplays();
}

void MainWindow::updateBatteryStatusDisplay()
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

void MainWindow::setupOptionsPanel()
{
    MachineTool& machineTool = MachineTool::getInstance();

    QStringList optionsNames = machineTool.getRepository().getOptionsLabels();

    for(auto optionName : optionsNames)
    {
        QPushButton* itemWidget = new QPushButton(optionName, ui->optionsListWidget);
        QListWidgetItem* item = new QListWidgetItem();
        item->setSizeHint(itemWidget->sizeHint());
        ui->optionsListWidget->addItem(item);
        ui->optionsListWidget->setItemWidget(item, itemWidget);
    }

    ui->optionsListWidget->setStyleSheet("QListWidget { background-color: transparent; }");
}

void MainWindow::hideWidgets()
{
    ui->smlEditorTab->setEnabled(false);
    //ui->programEditorTabWidget->removeTab(0);
}

void MainWindow::onMachineTool_ErrorStateChanged(QList<ERROR_CODE> errors)
{
    MachineTool& machineTool = MachineTool::getInstance();

    ui->serverPortLcdNumber->display(machineTool.getAdapterServerPort());
    ui->sensorsBufferSizeLcdNumber->display(machineTool.getRepository().getSensorsBufferSize());
    ui->devicesBufferSizeLcdNumber->display(machineTool.getRepository().getDevicesBufferSize());
    ui->currentConnectionsListWidget->clear();
    ui->currentConnectionsListWidget->addItems(machineTool.getConnectedAdapters());

    bool enableWidgets = false;
    if(errors.length() == 0)
    {
        enableWidgets = true;
        ui->statusBar->setStyleSheet("background-color: #333; color: #33bb33");
        ui->statusBar->showMessage("System is ready");
    }
    else
    {
        ui->statusBar->setStyleSheet("background-color: #333; color: #b22222");
        QString errorString = QStringLiteral("Error code:");
        for(auto error : errors)
        {
            errorString += " #" + QString::number(error);
        }
        ui->statusBar->showMessage(errorString);
    }

    ui->optionsListWidget->setEnabled(enableWidgets);
    ui->spindelsListWidget->setEnabled(enableWidgets);
    ui->supportDevicesListWidget->setEnabled(enableWidgets);

    ui->currentCoordinatesListWidget->setEnabled(enableWidgets);
    ui->baseCoordinatesListWidget->setEnabled(enableWidgets);
    ui->parkCoordinatesListWidget->setEnabled(enableWidgets);
    ui->edgesControlCheckBox->setEnabled(enableWidgets);

    ui->toBasePushButton->setEnabled(enableWidgets);

    if(enableWidgets && machineTool.getBased())
    {
        this->enableMotionWidgets();
    }
    else
    {
        this->disableMotionWidgets();
    }
}

void MainWindow::setMovementButtonsShortcutsAutoRepeat(bool state)
{
    for (auto i = m_axisesShortcuts.begin(); i != m_axisesShortcuts.end(); i++)
        (*i)->setAutoRepeat(state);
}

void MainWindow::setMovementButtonsAutoRepeat(bool state)
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
        ui->movementANegativePushButton,

        ui->movementBPositivePushButton,
        ui->movementBNegativePushButton,

        ui->movementCPositivePushButton,
        ui->movementCNegativePushButton,

        ui->movementUPositivePushButton,
        ui->movementUNegativePushButton,

        ui->movementVPositivePushButton,
        ui->movementVNegativePushButton,

        ui->movementWPositivePushButton,
        ui->movementWNegativePushButton
    };

    for (std::vector<QPushButton*>::iterator i = movementButtons.begin(); i != movementButtons.end(); i++)
        (*i)->setAutoRepeat(state);

    setMovementButtonsShortcutsAutoRepeat(state);
}

void MainWindow::enableMotionWidgets()
{
    this->setMotionWidgetsState(true);
}

void MainWindow::disableMotionWidgets()
{
    this->setMotionWidgetsState(false);
}

void MainWindow::setMotionWidgetsState(bool enableWidgets)
{
    ui->toZeroPushButton->setEnabled(enableWidgets);
    ui->toParkPushButton->setEnabled(enableWidgets);
    ui->zeroPushButton->setEnabled(enableWidgets);
    ui->zeroSensorPushButton->setEnabled(enableWidgets);
    ui->parkPushButton->setEnabled(enableWidgets);

    ui->runPushButton->setEnabled(enableWidgets);
}



void MainWindow::on_discreteRadioButton_1_clicked()
{
    MachineTool::getInstance().getRepository().setMovementStep(0.01);
    this->setMovementButtonsAutoRepeat(false);
}

void MainWindow::on_discreteRadioButton_2_clicked()
{
    MachineTool::getInstance().getRepository().setMovementStep(0.1);
    this->setMovementButtonsAutoRepeat(false);
}

void MainWindow::on_discreteRadioButton_3_clicked()
{
    MachineTool::getInstance().getRepository().setMovementStep(1);
    this->setMovementButtonsAutoRepeat(false);
}

void MainWindow::on_discreteRadioButton_4_clicked()
{
    MachineTool::getInstance().getRepository().setMovementStep(10);
    this->setMovementButtonsAutoRepeat(false);
}

void MainWindow::on_discreteRadioButton_5_clicked()
{
    MachineTool::getInstance().getRepository().setMovementStep(0.01);
    this->setMovementButtonsAutoRepeat(true);
}

void MainWindow::on_movementXPositivePushButton_clicked()
{
    MachineTool &i = MachineTool::getInstance();
    i.stepMove({
        std::make_pair<QString, double> ("X", i.getRepository().getMovementStep())
    });
}

void MainWindow::on_movementXNegativePushButton_clicked()
{
    MachineTool &i = MachineTool::getInstance();
    i.stepMove({
        std::make_pair<QString, double> ("X", -i.getRepository().getMovementStep())
    });
}

void MainWindow::on_movementYPositivePushButton_clicked()
{
    MachineTool &i = MachineTool::getInstance();
    i.stepMove({
        std::make_pair<QString, double> ("Y", i.getRepository().getMovementStep())
    });
}

void MainWindow::on_movementYNegativePushButton_clicked()
{
    MachineTool &i = MachineTool::getInstance();
    i.stepMove({
        std::make_pair<QString, double> ("Y", -i.getRepository().getMovementStep())
    });
}

void MainWindow::on_movementXNegativeYPositivePushButton_clicked()
{
    MachineTool &i = MachineTool::getInstance();
    i.stepMove({
        std::make_pair<QString, double> ("X", -i.getRepository().getMovementStep()),
        std::make_pair<QString, double> ("Y", i.getRepository().getMovementStep())
    });
}

void MainWindow::on_movementXPositiveYPositivePushButton_clicked()
{
    MachineTool &i = MachineTool::getInstance();
    i.stepMove({
        std::make_pair<QString, double> ("X", i.getRepository().getMovementStep()),
        std::make_pair<QString, double> ("Y", i.getRepository().getMovementStep())
    });
}

void MainWindow::on_movementXNegativeYNegativePushButton_clicked()
{
    MachineTool &i = MachineTool::getInstance();
    i.stepMove({
        std::make_pair<QString, double> ("X", -i.getRepository().getMovementStep()),
        std::make_pair<QString, double> ("Y", -i.getRepository().getMovementStep())
    });
}

void MainWindow::on_movementXPositiveYNegativePushButton_clicked()
{
    MachineTool &i = MachineTool::getInstance();
    i.stepMove({
        std::make_pair<QString, double> ("X", i.getRepository().getMovementStep()),
        std::make_pair<QString, double> ("Y", -i.getRepository().getMovementStep())
    });
}

void MainWindow::on_movementZPositivePushButton_clicked()
{
    MachineTool &i = MachineTool::getInstance();
    i.stepMove({
        std::make_pair<QString, double> ("Z", i.getRepository().getMovementStep())
    });
}

void MainWindow::on_movementZNegativePushButton_clicked()
{
    MachineTool &i = MachineTool::getInstance();
    i.stepMove({
        std::make_pair<QString, double> ("Z", -i.getRepository().getMovementStep())
    });
}

void MainWindow::on_movementAPositivePushButton_clicked()
{
    MachineTool &i = MachineTool::getInstance();
    i.stepMove({
        std::make_pair<QString, double> ("A", i.getRepository().getMovementStep())
    });
}

void MainWindow::on_movementANegativePushButton_clicked()
{
    MachineTool &i = MachineTool::getInstance();
    i.stepMove({
        std::make_pair<QString, double> ("A", -i.getRepository().getMovementStep())
    });
}

void MainWindow::on_movementBNegativePushButton_clicked()
{
    MachineTool &i = MachineTool::getInstance();
    i.stepMove({
        std::make_pair<QString, double> ("B", -i.getRepository().getMovementStep())
    });
}

void MainWindow::on_movementBPositivePushButton_clicked()
{
    MachineTool &i = MachineTool::getInstance();
    i.stepMove({
        std::make_pair<QString, double> ("B", i.getRepository().getMovementStep())
    });
}

void MainWindow::on_movementCNegativePushButton_clicked()
{
    MachineTool &i = MachineTool::getInstance();
    i.stepMove({
        std::make_pair<QString, double> ("C", -i.getRepository().getMovementStep())
    });
}

void MainWindow::on_movementCPositivePushButton_clicked()
{
    MachineTool &i = MachineTool::getInstance();
    i.stepMove({
        std::make_pair<QString, double> ("C", i.getRepository().getMovementStep())
    });
}

void MainWindow::on_movementUNegativePushButton_clicked()
{
    MachineTool &i = MachineTool::getInstance();
    i.stepMove({
        std::make_pair<QString, double> ("U", -i.getRepository().getMovementStep())
    });
}

void MainWindow::on_movementUPositivePushButton_clicked()
{
    MachineTool &i = MachineTool::getInstance();
    i.stepMove({
        std::make_pair<QString, double> ("U", i.getRepository().getMovementStep())
    });
}

void MainWindow::on_movementVNegativePushButton_clicked()
{
    MachineTool &i = MachineTool::getInstance();
    i.stepMove({
        std::make_pair<QString, double> ("V", -i.getRepository().getMovementStep())
    });
}

void MainWindow::on_movementVPositivePushButton_clicked()
{
    MachineTool &i = MachineTool::getInstance();
    i.stepMove({
        std::make_pair<QString, double> ("V", i.getRepository().getMovementStep())
    });
}

void MainWindow::on_movementWNegativePushButton_clicked()
{
    MachineTool &i = MachineTool::getInstance();
    i.stepMove({
        std::make_pair<QString, double> ("W", -i.getRepository().getMovementStep())
    });
}

void MainWindow::on_movementWPositivePushButton_clicked()
{
    MachineTool &i = MachineTool::getInstance();
    i.stepMove({
        std::make_pair<QString, double> ("W", i.getRepository().getMovementStep())
    });
}

void MainWindow::on_feedrateScrollBar_valueChanged(int value)
{
    MachineTool& machineTool = MachineTool::getInstance();
    machineTool.getRepository().setVelocity(value);
    ui->feedrateLcdNumber->display(QString::number(machineTool.getRepository().getVelocity()));
}

void MainWindow::on_exit_action_triggered()
{
    exit(0);
}

void MainWindow::on_pointsAmountToolButton_clicked()
{
    QMessageBox(QMessageBox::Information, "Количество точек", QString::number(ui->pointsTableWidget->rowCount())).exec();
}

void MainWindow::on_pointAddToolButton_clicked()
{
    addPoint();
}

void MainWindow::on_pointDeleteToolButton_clicked()
{
    QItemSelectionModel *select = ui->pointsTableWidget->selectionModel();
    QModelIndexList selectedItemsIndexes = select->selectedIndexes();

    if(selectedItemsIndexes.size() > 0)
    {
        QModelIndexList selectedRowsIndexes = PointsTableWidget::getRowsIndexes(selectedItemsIndexes);
        deletePoints(selectedRowsIndexes);
    }
}

void MainWindow::on_pointCursorToolButton_clicked()
{
    PointsTableWidget* currentTableWidget = ui->pointsTableWidget;
    if(currentTableWidget->rowCount() > 0)
    {
        ToSelectionPointDialog(currentTableWidget, this).exec();
    }
}

void MainWindow::on_pointEditToolButton_clicked()
{
    QItemSelectionModel *select = ui->pointsTableWidget->selectionModel();
    if(select->hasSelection())
    {
         editPoint(select->currentIndex());
    }
    else
    {
         QMessageBox(QMessageBox::Information, "Сообщение", QString("Точка не выбрана")).exec();
    }
}

void MainWindow::on_pointCopyToolButton_clicked()
{
    MachineTool& machineTool = MachineTool::getInstance();

    QItemSelectionModel *select = ui->pointsTableWidget->selectionModel();
    QModelIndexList selectedItemsIndexes = select->selectedIndexes();
    if(selectedItemsIndexes.size() > 0)
    {
        QModelIndexList selectedRowsIndexes = PointsTableWidget::getRowsIndexes(selectedItemsIndexes);

        for(auto row : selectedRowsIndexes)
        {
            Point p = machineTool.getRepository().getPoint(static_cast<unsigned int>(row.row()));
            machineTool.getRepository().addPoint(p.coordsMap());
        }
    }
    else
    {
        return;
    }
}

void MainWindow::onMachineTool_EdgesControlStatusChanged(bool state)
{
    ui->edgesControlCheckBox->setChecked(state);
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

void MainWindow::addPoint()
{
    MachineTool& machineTool = MachineTool::getInstance();
    AddPointDialog(machineTool.getRepository(), this).exec();
}

void MainWindow::editPoint(QModelIndex index)
{
    MachineTool& machineTool = MachineTool::getInstance();

    try
    {
        AddPointDialog(machineTool.getRepository(), static_cast<unsigned int>(index.row()), this).exec();
    }
    catch(std::out_of_range e)
    {
        QMessageBox(QMessageBox::Warning, "Ошибка", e.what()).exec();
    }
}

void MainWindow::deletePoints(QModelIndexList indexes)
{
    MachineTool& machineTool = MachineTool::getInstance();

    for(int i = indexes.size() - 1; i >= 0; i--)
    {
        machineTool.getRepository().deletePoint(static_cast<unsigned int>(indexes[i].row()));
    }
}

void MainWindow::onPointsUpdated()
{
    MachineTool& machineTool = MachineTool::getInstance();

    QList<Point> points = machineTool.getRepository().getPoints();
    QList<PointsTableWidget*> fields = { ui->pointsTableWidget };
    QStringList axisesLabels = machineTool.getRepository().getAxisesNames();

    for(auto field : fields)
    {
        field->clear();
        field->setColumnCount(axisesLabels.size());
        field->setHorizontalHeaderLabels(axisesLabels);
        field->setRowCount(points.size());

        for(int i = 0; i < points.size(); i++)
        {
            QList< QPair<QString, double> > coords = points[i].coords();
            int j = 0;
            for(auto coordsItem : coords)
            {
                field->setItem(i, j, new QTableWidgetItem(QString::number(coordsItem.second)));
                j++;
            }
        }
    }

    // растянуть таблицу с координатами редактора точек
    for(auto field : fields)
    {
        for (int i = 0; i < field->columnCount(); i++)
        {
            field->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
        }
    }

    QList<QToolButton*> pointsActionsButtons =
    {
        ui->pointDeleteToolButton,
        ui->pointEditToolButton,
        ui->pointCopyToolButton,
        ui->pointCursorToolButton,
        ui->pointTransitionToolButton
    };

    if(ui->pointsTableWidget->rowCount() > 0)
    {
        for(auto button : pointsActionsButtons)
        {
            button->setEnabled(true);
        }
    }
    else
    {
        for(auto button : pointsActionsButtons)
        {
            button->setEnabled(false);
        }
    }
}

void MainWindow::on_open_action_triggered()
{
    MachineTool& machineTool = MachineTool::getInstance();

    if(ui->smlEditorTab->isVisible())
    {
        //mainWindowController->openSMLFile();
    }
    else
    {
        if(ui->gcodesEditorTab->isVisible())
        {
            machineTool.getRepository().openGCodesFile();
        }
    }
}

void MainWindow::on_gcodesEditorPlainTextEdit_textChanged()
{
    QString text = ui->gcodesEditorPlainTextEdit->toPlainText();
    //MachineTool& machineTool = MachineTool::getInstance();
    //machineTool.getRepository().setGCodes(text);
}

void MainWindow::on_importsettings_action_triggered()
{
    MachineTool& machineTool = MachineTool::getInstance();

    machineTool.getRepository().importSettings();
}

void MainWindow::on_savesettings_action_triggered()
{
    MachineTool& machineTool = MachineTool::getInstance();

    machineTool.getRepository().exportSettings();
}

void MainWindow::on_add_action_triggered()
{
    /*if(ui->smlEditorTab->isVisible())
    {
        //mainWindowController->addSMLFile();
    }
    else
    {
        if(ui->gcodesEditorTab->isVisible())
        {
            m_machineTool->addGCodesFile(ui->gcodesEditorPlainTextEdit->toPlainText());
        }
    }*/
}

void MainWindow::on_create_action_triggered()
{
    MachineTool& machineTool = MachineTool::getInstance();

    if(ui->smlEditorTab->isVisible())
    {
        //mainWindowController->newSMLFile();
    }
    else
    {
        if(ui->gcodesEditorTab->isVisible())
        {
            machineTool.getRepository().newGCodesFile();
        }
    }
}

void MainWindow::on_save_action_triggered()
{
    MachineTool& machineTool = MachineTool::getInstance();

    if(ui->smlEditorTab->isVisible())
    {
        //mainWindowController->saveSMLFile();
    }
    else
    {
        if(ui->gcodesEditorTab->isVisible())
        {
            machineTool.getRepository().saveGCodesFile(ui->gcodesEditorPlainTextEdit->toPlainText());
        }
    }
}

void MainWindow::on_saveas_action_triggered()
{
    MachineTool& machineTool = MachineTool::getInstance();

    if(ui->smlEditorTab->isVisible())
    {
        //mainWindowController->saveSMLFileAs();
    }
    else
    {
        if(ui->gcodesEditorTab->isVisible())
        {
            machineTool.getRepository().saveGCodesFileAs(ui->gcodesEditorPlainTextEdit->toPlainText());
        }
    }
}

void MainWindow::on_view_action_triggered()
{
    try
    {
        MachineTool& machineTool = MachineTool::getInstance();
        machineTool.getRepository().setGCodes(ui->gcodesEditorPlainTextEdit->toPlainText());

        if(machineTool.getRepository().getFilePath().isEmpty())
        {
            GCodesViewInteractor::execute(machineTool.getRepository().getGCodesProgram(), this);
        }
        else
        {
            GCodesViewInteractor::execute(machineTool.getRepository().getFilePath(), this);
        }
    }
    catch(InvalidConfigurationException e)
    {
        qDebug() << "MainWindow::on_view_action_triggered:" << e.message();
        QMessageBox(QMessageBox::Warning, "Ошибка", e.message()).exec();
    }
    catch(...)
    {
        qDebug() << "MainWindow::on_view_action_triggered: unknown error";
        QMessageBox(QMessageBox::Warning, "Ошибка", "Ошибка визуализации УП");
    }
}

void MainWindow::on_consoleOpenPushButton_clicked()
{
    MachineTool& machineTool = MachineTool::getInstance();

    SMLConsoleDialog(machineTool, this).exec();
}

/*void MainWindow::deleteSelectedCommands(QModelIndexList indexes)
{
    std::vector<int> rows;
    for(auto index : indexes)
    {
        rows.push_back(index.row());
    }

    std::sort(rows.begin(), rows.begin() + rows.size());
    std::reverse(rows.begin(), rows.begin() + rows.size());

    for(auto row : rows)
    {
        mainWindowController->deleteCommand(row);
    }
}*/

/*void MainWindow::on_smlEditorTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    if(column != 1)
    {
        column = 1;
    }
    QString commandName = item->text(column);
    int commandNumber = mainWindowController->getCommandId(commandName);

    unsigned int currentCommandNumber = item->text(0).toUInt() - 1;
    switch (commandNumber) {
    case CMD_SWITCH_ON:
        SMLSwitchOnDialog(mainWindowController, currentCommandNumber, this, true).exec();
        break;
    case CMD_SWITCH_OFF:
        SMLSwitchOffDialog(mainWindowController, currentCommandNumber, this, true).exec();
        break;
    case CMD_COMMENT:
        SMLCommentDialog(mainWindowController, currentCommandNumber, this, true).exec();
        break;
    case CMD_PAUSE:
        SMLPauseDialog(mainWindowController, currentCommandNumber, this, true).exec();
        break;
    case CMD_LINE:
        LineDialog(mainWindowController, currentCommandNumber, this, true).exec();
        break;
    case CMD_ARC:
        ArcDialog(mainWindowController, currentCommandNumber, this, true).exec();
        break;
    case CMD_TTLINE:
        SMLTTLineDialog(mainWindowController, currentCommandNumber, this, true).exec();
        break;
    case CMD_VARIABLE:
        SMLVariableDialog(mainWindowController, currentCommandNumber, this, true).exec();
        break;
    case CMD_TTTARC:
        SMLTTTArcDialog(mainWindowController, currentCommandNumber, this, true).exec();
        break;
    default:
        QMessageBox(QMessageBox::Warning, "Ошибка", "Выбранная команда не может быть отредактирована").exec();
        break;
    }
}*/

/*void MainWindow::commandsCopySlot()
{
    qDebug() << "Copy signal received";
}*/

/*void MainWindow::commandsCutSlot()
{
    qDebug() << "Cut signal received";
}*/

/*void MainWindow::commandsPasteSlot()
{
    qDebug() << "Paste signal received";
}*/

/*void MainWindow::commandsUndoSlot()
{
    qDebug() << "Undo signal received";
}*/

/*void MainWindow::on_commandsToolsListWidget_clicked(const QModelIndex &index)
{
    QString commandName = index.data().toString();
    int commandNumber = mainWindowController->getCommandId(commandName);

    size_t currentCommandNumber = mainWindowController->getCommandsCount();

    QList<QTreeWidgetItem*> selectedItems = ui->smlEditorTreeWidget->selectedItems();
    if(selectedItems.size() > 0)
    {
        currentCommandNumber = selectedItems[0]->text(0).toInt()-1;
    }

    switch (commandNumber) {
    case CMD_SWITCH_ON:
        SMLSwitchOnDialog(mainWindowController, currentCommandNumber, this).exec();
        break;
    case CMD_SWITCH_OFF:
        SMLSwitchOffDialog(mainWindowController, currentCommandNumber, this).exec();
        break;
    case CMD_COMMENT:
        SMLCommentDialog(mainWindowController, currentCommandNumber, this).exec();
        break;
    case CMD_PAUSE:
        SMLPauseDialog(mainWindowController, currentCommandNumber, this).exec();
        break;
    case CMD_LINE:
        LineDialog(mainWindowController, currentCommandNumber, this).exec();
        break;
    case CMD_ARC:
        ArcDialog(mainWindowController, currentCommandNumber, this).exec();
        break;
    case CMD_TTLINE:
        SMLTTLineDialog(mainWindowController, currentCommandNumber, this).exec();
        break;
    case CMD_VARIABLE:
        SMLVariableDialog(mainWindowController, currentCommandNumber, this).exec();
        break;
    case CMD_TTTARC:
        SMLTTTArcDialog(mainWindowController, currentCommandNumber, this).exec();
        break;
    default:
        QMessageBox(QMessageBox::Warning, "Ошибка", "Неизвестная команда").exec();
        break;
    }
}*/

/*void MainWindow::updateCommandsEditorWidgets()
{
    updateSMLCommandsTreeWidget();
    updateGCodesEditorWidget();
}*/

/*void MainWindow::updateSMLCommandsTreeWidget()
{
    ui->smlEditorTreeWidget->clear();
    QList<QTreeWidgetItem *> items = mainWindowController->getCommands();
    ui->smlEditorTreeWidget->addTopLevelItems(items);

    for(int i = 0; i < ui->smlEditorTreeWidget->columnCount() - 1; i++)
    {
        ui->smlEditorTreeWidget->resizeColumnToContents(i);
    }
}*/

void MainWindow::on_edgesControlCheckBox_clicked()
{
    MachineTool& machineTool = MachineTool::getInstance();

    machineTool.getRepository().setSoftLimitsMode(ui->edgesControlCheckBox->isChecked());
}

void MainWindow::on_syntaxHighlightingCheckBox_clicked()
{
    if(ui->syntaxHighlightingCheckBox->isChecked())
    {
        // настройка подсветки синтаксиса
        m_hightlighter->setDocument(ui->gcodesEditorPlainTextEdit->document());
        m_hightlighter->setPattern();
    }
    else
    {
        m_hightlighter->setDocument(nullptr);
        m_hightlighter->setPattern();
    }
}

void MainWindow::on_runPushButton_clicked()
{
    MachineTool& machineTool = MachineTool::getInstance();
    if(machineTool.isProgramEmpty())
    {
        QMessageBox(QMessageBox::Information, "Отсутствует управляющая программа", "Нет управляющей программы").exec();
        return;
    }

    if(!machineTool.isGCodesCorrect())
    {
        QMessageBox(QMessageBox::Critical, "Ошибка", "Возникла ошибка при подготовке управляющей программы к выполнению").exec();
        return;
    }

    ProgramProcessingDialog(this).exec();
}

void MainWindow::on_toBasePushButton_clicked()
{
    MachineTool& machineTool = MachineTool::getInstance();
    machineTool.moveToBase();
}

void MainWindow::on_zeroPushButton_clicked()
{
    MachineTool& machineTool = MachineTool::getInstance();
    machineTool.getRepository().setZeroCoordinates(machineTool.getRepository().getCurrentPositionFromBase());
    this->updateCoordinatesDisplays();
}

void MainWindow::on_toZeroPushButton_clicked()
{
    MachineTool& machineTool = MachineTool::getInstance();
    machineTool.moveToPoint(machineTool.getRepository().getZeroCoordinates());
}

void MainWindow::on_parkPushButton_clicked()
{
    MachineTool& machineTool = MachineTool::getInstance();
    machineTool.getRepository().setParkCoordinates(machineTool.getRepository().getCurrentPositionFromBase());
    this->updateCoordinatesDisplays();
}

void MainWindow::on_toParkPushButton_clicked()
{
    MachineTool& machineTool = MachineTool::getInstance();
    machineTool.moveToPoint(machineTool.getRepository().getParkCoordinates());
}
