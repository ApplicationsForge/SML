#include "servermanager.h"

ServerManager::ServerManager(const SettingsManager &settingsManager, QObject *parent) :
    QObject(parent),
    m_server(new SMLServer(settingsManager, this))
{
    try
    {
        size_t sensorsPackageSize = settingsManager.get("MachineToolInformation", "SensorsBufferSize").toUInt();
        size_t devicesPackageSize = settingsManager.get("MachineToolInformation", "DevicesBufferSize").toUInt();
        m_u1CurrentState = U1State(sensorsPackageSize, devicesPackageSize);
    }
    catch(std::invalid_argument e)
    {
        QMessageBox(QMessageBox::Warning, "Settings Error", e.what()).exec();
    }

    setupConnections();
}

ServerManager::~ServerManager()
{
    resetConnections();
}

void ServerManager::setupConnections()
{
    QObject::connect(m_server.data(), SIGNAL(byteMessageReceived(QByteArray)), this, SLOT(onBinaryMessageReceived(QByteArray)));
    QObject::connect(m_server.data(), SIGNAL(u1Connected()), this, SLOT(onU1Connected()));
    QObject::connect(m_server.data(), SIGNAL(u1Disconnected()), this, SLOT(onU1Disconnected()));
    QObject::connect(m_server.data(), SIGNAL(u2Connected()), this, SLOT(onU2Connected()));
    QObject::connect(m_server.data(), SIGNAL(u2Disconnected()), this, SLOT(onU2Disconnected()));
}

void ServerManager::resetConnections()
{
    QObject::disconnect(m_server.data(), SIGNAL(byteMessageReceived(QByteArray)), this, SLOT(onBinaryMessageReceived(QByteArray)));
    QObject::disconnect(m_server.data(), SIGNAL(u1Connected()), this, SLOT(onU1Connected()));
    QObject::disconnect(m_server.data(), SIGNAL(u1Disconnected()), this, SLOT(onU1Disconnected()));
    QObject::disconnect(m_server.data(), SIGNAL(u2Connected()), this, SLOT(onU2Connected()));
    QObject::disconnect(m_server.data(), SIGNAL(u2Disconnected()), this, SLOT(onU2Disconnected()));
}

void ServerManager::updateU1State(QList<QVariant> sensorsState, QList<QVariant> devicesState, int lastError)
{
    byte_array currentSensorsState;
    for(auto group : sensorsState)
    {
        currentSensorsState.push_back(group.toUInt());
    }

    byte_array currentDevicesState;
    for(auto group : devicesState)
    {
        currentDevicesState.push_back(group.toUInt());
    }

    updateU1State(currentSensorsState, currentDevicesState, lastError);
}

void ServerManager::updateU1State(byte_array sensorsState, byte_array devicesState, int lastError)
{
    m_u1CurrentState.setLastEror(lastError);
    m_u1CurrentState.setSensorsState(sensorsState);
    m_u1CurrentState.setDevicesState(devicesState);
    emit u1StateIsChanged();

    int currentU1Error = m_u1CurrentState.getLastError();
    if(currentU1Error != 0)
    {
        emit u1ErrorIsOccured(m_u1CurrentState.getLastError());
    }
}

void ServerManager::switchDeviceOn(ServerManager::DeviceType deviceType, QString index, QStringList params)
{
    QtJson::JsonObject generalMessage;
    QtJson::JsonObject u1Message;
    QtJson::JsonObject device;
    device["Index"] = index;
    device["Target"] = "On";

    switch (deviceType) {
    case DeviceType::Spindel:
        device["Type"] = "Spindel";
        if(params.size() > 0)
        {
            device["Rotations"] = params[0];
        }
        break;
    case DeviceType::Support:
        device["Type"] = "Support";
        break;
    default:
        break;
    }
    u1Message["SwitchDevice"] = device;
    generalMessage["MessageToU1"] = u1Message;

    bool ok = false;
    QByteArray message = QtJson::serialize(generalMessage, ok);
    qDebug() << "Try to switch on device =" << message;
    if(ok)
    {
        m_server->sendMessageToU1(message);
    }
}

void ServerManager::switchDeviceOff(DeviceType deviceType, QString index)
{
    QtJson::JsonObject generalMessage;
    QtJson::JsonObject u1Message;
    QtJson::JsonObject device;
    device["Index"] = index;
    device["Target"] = "Off";
    switch (deviceType) {
    case DeviceType::Spindel:
        device["Type"] = "Spindel";
        break;
    case DeviceType::Support:
        device["Type"] = "Support";
        break;
    default:
        break;
    }
    u1Message["SwitchDevice"] = device;
    generalMessage["MessageToU1"] = u1Message;

    bool ok = false;
    QByteArray message = QtJson::serialize(generalMessage, ok);
    qDebug() << "Try to switch off device =" << message;
    if(ok)
    {
        m_server->sendMessageToU1(message);
    }
}

void ServerManager::switchDeviceOn(byte_array data)
{
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
        m_server->sendMessageToU1(message);
    }
}

byte_array ServerManager::getSensorsState()
{
    return m_u1CurrentState.getSensorsState();
}

byte_array ServerManager::getDevicesState()
{
    return m_u1CurrentState.getDevicesState();
}

void ServerManager::startServer()
{
    m_server->start();
}

void ServerManager::stopServer()
{
    m_server->stop();
}

QStringList ServerManager::getCurrentConnections()
{
    return m_server->currentAdapters();
}

size_t ServerManager::getServerPort()
{
    return m_server->port();
}

size_t ServerManager::getSensorsBufferSize()
{
    return m_u1CurrentState.getSensorsState().size();
}

size_t ServerManager::getDevicesBufferSize()
{
    return m_u1CurrentState.getDevicesState().size();
}

void ServerManager::onBinaryMessageReceived(QByteArray message)
{
    bool ok;
    QString json = QString::fromUtf8(message);
    QtJson::JsonObject result = QtJson::parse(json, ok).toMap();
    if(ok)
    {
        QtJson::JsonObject u1State = result["U1State"].toMap();
        if(!u1State.isEmpty())
        {
            QList<QVariant> sensorsState = u1State["SensorsState"].toList();
            //qDebug() << sensorsState;
            QList<QVariant> devicesState = u1State["DevicesState"].toList();
            //qDebug() << devicesState;
            int lastError = u1State["LastError"].toInt();
            updateU1State(sensorsState, devicesState, lastError);
        }

        //QtJson::JsonObject u2State = result["U2State"].toMap();
        //qDebug() << !(u2State.isEmpty());
    }
    else
    {
        qDebug() << "an error is occured during parsing json" << QString::fromUtf8(message);
    }
}

void ServerManager::onU1Connected()
{
    emit u1Connected();
}

void ServerManager::onU1Disconnected()
{
    emit u1Disconnected();
}

void ServerManager::onU2Connected()
{
    emit u2Connected();
}

void ServerManager::onU2Disconnected()
{
    emit u2Disconnected();
}
