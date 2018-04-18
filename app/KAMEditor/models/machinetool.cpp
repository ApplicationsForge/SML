#include "machinetool.h"

MachineTool::MachineTool(QObject *parent) :
    QObject(parent),
    m_settingsManager(new SettingsManager()),
    m_serverManager(new ServerConnectionManager(m_settingsManager, false, this)),
    m_sensorsManager(new SensorsManager(m_settingsManager)),
    m_devicesManager(new DevicesManager(m_settingsManager)),
    m_axisesManager(new AxisesManager(m_settingsManager)),
    m_gcodesFilesManager(new GCodesFilesManager(this)),
    m_gcodesManager(new GCodesManager()),
    m_pointsManager(new PointsManager())
{
    connect(m_serverManager, SIGNAL(u1StateIsChanged()), this, SLOT(updateU1State()));
    connect(m_serverManager, SIGNAL(textMessageReceived(QString)), this, SLOT(onMessageReceived(QString)));
    connect(m_serverManager, SIGNAL(binaryMessageReceived(QByteArray)), this, SLOT(onMessageReceived(QByteArray)));

    connect(m_serverManager, SIGNAL(serverIsConnected()), this, SLOT(onConnected()));
    connect(m_serverManager, SIGNAL(serverIsConnected()), this, SLOT(getInitialU1State()));
    connect(m_serverManager, SIGNAL(serverIsDisconnected(QString)), this, SLOT(onDisconnected(QString)));
}

MachineTool::~MachineTool()
{
    delete m_pointsManager;
    delete m_gcodesManager;
    delete m_gcodesFilesManager;
    delete m_axisesManager;
    delete m_devicesManager;
    delete m_sensorsManager;
    delete m_serverManager;
    delete m_settingsManager;
}

void MachineTool::onConnected()
{
    updateU1State();
}

void MachineTool::onDisconnected(QString message)
{
    emit machineToolIsDisconnected(message);
}

void MachineTool::updateU1State()
{
    byte_array sensors = m_serverManager->getSensorsState();
    byte_array devices = m_serverManager->getDevicesState();
    m_sensorsManager->updateSensors(sensors);
    m_devicesManager->updateDevices(devices);
    emit u1StateIsChanged();
}

void MachineTool::getInitialU1State()
{
    QtJson::JsonObject generalMessage;
    QtJson::JsonObject u1Message;

    u1Message["DirectMessage"] = "GetState";
    generalMessage["MessageToU1"] = u1Message;

    bool ok = false;
    QByteArray message = QtJson::serialize(generalMessage, ok);
    qDebug() << "Get U1 state =" << message;
    if(ok)
    {
        m_serverManager->sendBinaryMessage(message);
    }
}

void MachineTool::sendTextMessgeToServer(QString message)
{
    if(!m_serverManager->sendTextMessage(message))
    {
        QMessageBox(QMessageBox::Warning,
                    "Ошибка подключения",
                    QString("Не могу отправить на серевер сообщение") + message).exec();
    }
}

void MachineTool::sendBinaryMessageToServer(QByteArray message)
{
    if(!m_serverManager->sendBinaryMessage(message))
    {
        QMessageBox(QMessageBox::Warning,
                    "Ошибка подключения",
                    QString("Не могу отправить на серевер сообщение") + QString::fromUtf8(message)).exec();
    }
}

void MachineTool::onMessageReceived(QString message)
{
    emit receivedMessage(message);
}

void MachineTool::onMessageReceived(QByteArray message)
{
    emit receivedMessage(QString::fromUtf8(message));
}

void MachineTool::openWebSocketConnection()
{
    m_serverManager->openWebSocket();
}

void MachineTool::closeWebSocketConnection()
{
    m_serverManager->closeWebSocket();
}

void MachineTool::onGCodesLoadingStart()
{
    emit gcodesLoadingStart();
}

void MachineTool::onGCodesLoading(int currentValue)
{
    emit gcodesIsLoading(currentValue);
}

void MachineTool::onGCodesLoaded()
{
    emit gcodesLoaded();
}

void MachineTool::exportSettings()
{
    QString path = QFileDialog::getSaveFileName(0, "Выберите путь до файла", "", "*.ini");
    m_settingsManager->exportSettings(path);
}

void MachineTool::importSettings()
{
    QString path = QFileDialog::getOpenFileName(0, "Выберите файл с настройками", "", "*.ini");
    m_settingsManager->importSettings(path);
}

void MachineTool::parseGCodes(QString data)
{
    m_gcodesManager->updateGCodesProgram(data);
}

void MachineTool::setSoftLimitsMode(bool enable)
{
    m_axisesManager->setSoftLimitsMode(enable);
}

void MachineTool::switchDevice(QString deviceName)
{
    try
    {
        Device &device = m_devicesManager->findDevice(deviceName);
        qDebug() << "current state = " << device.getCurrentState();
        byte_array data = m_devicesManager->switchDeviceData(device, !device.getCurrentState());

        QtJson::JsonObject generalMessage;
        QtJson::JsonObject u1Message;
        QtJson::JsonArray u1Data;

        for(auto byte_unit : data)
        {
            u1Data.push_back(byte_unit);
        }
        u1Message["SwitchDevice"] = u1Data;
        generalMessage["MessageToU1"] = u1Message;


        bool ok = false;
        QByteArray message = QtJson::serialize(generalMessage, ok);
        qDebug() << "Try to switch device =" << message;
        if(ok)
        {
            m_serverManager->sendBinaryMessage(message);
        }
    }
    catch(std::invalid_argument e)
    {
        QMessageBox(QMessageBox::Warning, "Ошибка", e.what()).exec();
    }
}

void MachineTool::updateVelocity(int value)
{
    if(value >= 0)
    {
        feedrate = value;
    }
    else
    {
        QMessageBox(QMessageBox::Warning, "Ошибка", "Скорость не может быть отрицательной").exec();
    }
}

void MachineTool::updateSpindelRotations(int value)
{
    if(value >= 0)
    {
        rotations = value;
    }
    else
    {
        QMessageBox(QMessageBox::Warning, "Ошибка", "Обороты шпинделя не могут быть отрицательными").exec();
    }
}

void MachineTool::addPoint(QStringList coordinates)
{
    Point* p = PointsManager::makePoint(coordinates);
    m_pointsManager->addPoint(p);
    emit pointsUpdated();
}

void MachineTool::updatePoint(QStringList coordinates, unsigned int number)
{
    Point* p = PointsManager::makePoint(coordinates);
    try
    {
        std::shared_ptr<Point> originPoint = m_pointsManager->operator [](number);
        unsigned int originPointDimension = originPoint->size();
        unsigned int newPointDimension = p->size();
        unsigned int rangeForUpdate = std::min(originPointDimension, newPointDimension);
        for(unsigned int i = 0; i < rangeForUpdate; i++)
        {
            originPoint->get(i) = p->get(i);
        }
        emit pointsUpdated();
    }
    catch(std::out_of_range e)
    {
        QMessageBox(QMessageBox::Warning, "Ошибка", e.what()).exec();
    }
}

void MachineTool::deletePoint(unsigned int number)
{
    try
    {
        std::shared_ptr<Point> p = m_pointsManager->operator [](number);
        m_pointsManager->deletePoint(p);
        emit pointsUpdated();
    }
    catch(std::out_of_range e)
    {
        QMessageBox(QMessageBox::Warning, "Ошибка", e.what()).exec();
    }
}


void MachineTool::openGCodesFile()
{
    connect(m_gcodesFilesManager, SIGNAL(startLoading()), this, SLOT(onGCodesLoadingStart()));
    connect(m_gcodesFilesManager, SIGNAL(loading(int)), this, SLOT(onGCodesLoading(int)));
    connect(m_gcodesFilesManager, SIGNAL(loaded()), this, SLOT(onGCodesLoaded()));

    m_gcodesFilesManager->openGCodesFile();

    emit gcodesUpdated();
    emit filePathUpdated();
}

QString MachineTool::getGCodesFileContent()
{
    return m_gcodesFilesManager->getContent();
}

void MachineTool::saveGCodesFile(const QString data)
{

    m_gcodesFilesManager->setFileContent(data);
    m_gcodesFilesManager->saveGCodesFile();
    emit filePathUpdated();
}

void MachineTool::saveGCodesFileAs(const QString data)
{
    m_gcodesFilesManager->setFileContent(data);
    m_gcodesFilesManager->saveGCodesFileAs();
    emit filePathUpdated();
}

void MachineTool::newGCodesFile()
{
    m_gcodesFilesManager->newGCodesFile();
    emit gcodesUpdated();
    emit filePathUpdated();
}

void MachineTool::addGCodesFile(const QString data)
{
    saveGCodesFile(data);
    m_gcodesFilesManager->addGCodesFile();
    emit gcodesUpdated();
    emit filePathUpdated();
}

void MachineTool::updateGCodes(const QString &data)
{
    m_gcodesManager->setGcodes(data);
}

QString MachineTool::getGCodesProgram()
{
    return m_gcodesManager->gcodes();
}

QList<Point> MachineTool::getMachineToolCoordinates()
{
    QList<Point> machineToolCoordinates;
    machineToolCoordinates.push_back(m_axisesManager->getCurrentCoordinatesFromZero());
    machineToolCoordinates.push_back(m_axisesManager->getCurrentCoordinatesFromBase());
    machineToolCoordinates.push_back(m_axisesManager->getParkCoordinates());
    return machineToolCoordinates;
}

QStringList MachineTool::getSensorsLabels()
{
    return m_sensorsManager->sensorsLabels();
}

QStringList MachineTool::getSensorParametrLabels()
{
    return m_sensorsManager->sensorParametrLabels();
}

QList<QStringList> MachineTool::getSensorsSettings()
{
    return m_sensorsManager->sensorsSettings();
}

QList<QColor> MachineTool::getSensorsLeds()
{
    return m_sensorsManager->sensorsLeds();
}

QStringList MachineTool::getDevicesNames()
{
    return m_devicesManager->devicesNames();
}

QStringList MachineTool::getDevicesParametrsNames()
{
    return m_devicesManager->devicesParametrsNames();
}

QList<QStringList> MachineTool::getDevicesSettings()
{
    return m_devicesManager->devicesSettings();
}

QStringList MachineTool::getOnScreenDevicesNames()
{
    return m_devicesManager->onScreenDevicesNames();
}

QList<bool> MachineTool::getOnScreenDevicesStates()
{
    return m_devicesManager->onScreenDevicesStates();
}

QStringList MachineTool::getAxisesNames()
{
    return m_axisesManager->getAxisesNames();
}

QStringList MachineTool::getAxisesSettings()
{
    return m_axisesManager->getAxisesSettings();
}

QStringList MachineTool::getOptionsNames()
{
    //todo: переписсать метод через модель
    QStringList optionsNames =
    {
        "Кабриоль",
        "Датчик вылета инструмента",
        "Станция автоматической смазки"
    };
    return optionsNames;
}

unsigned int MachineTool::getVelocity()
{
    //return m_machineTool->getVelocity();
    return feedrate;
}

unsigned int MachineTool::getSpindelRotations()
{
    //return m_machineTool->getSpindelRotations();
    return rotations;
}

QList<QStringList> MachineTool::getPoints()
{
    return m_pointsManager->points();
}

QStringList MachineTool::getPoint(unsigned int number)
{
    return m_pointsManager->point(number);
}

QString MachineTool::getFilePath(QString type)
{
    QString path = "";
    if(type == "gcodes")
    {
        path = m_gcodesFilesManager->getFilePath();
    }
    else
    {
        if(type == "sml")
        {
            path = "sml file path";
        }
    }
    return path;
}