#include "arduino_u2_adapter.h"

ArduinoU2Adapter::ArduinoU2Adapter(QObject *parent) :
    QObject(parent),
    m_serial(new QSerialPort(this)),
    m_socketHandler(new WebSocketHandler("", this)),
    m_repository(this)
{
    m_serial->setBaudRate(9600);
    connect(m_serial, SIGNAL(readyRead()), this, SLOT(onQSerialPort_ReadyRead()));

    connect(m_socketHandler, SIGNAL(connected()), this, SLOT(onWebSocketHandler_Connected()));
    connect(m_socketHandler, SIGNAL(disconnected(QWebSocketProtocol::CloseCode,QString)), this, SLOT(onWebSocketHandler_Disconnected(QWebSocketProtocol::CloseCode,QString)));
    connect(m_socketHandler, SIGNAL(binaryMessageReceived(QByteArray)), this, SLOT(onWebSocketHandler_BinaryMessageReceived(QByteArray)));

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        qDebug() << "Name : " << info.portName();
        qDebug() << "Description : " << info.description();
        qDebug() << "Manufacturer: " << info.manufacturer();
        if(info.portName() == m_repository.m_serialPortName)
        {
            this->openSerialPort(info);
            break;
        }
    }
}

ArduinoU2Adapter::~ArduinoU2Adapter()
{
    disconnect(m_socketHandler, SIGNAL(connected()), this, SLOT(onWebSocketHandler_Connected()));
    disconnect(m_socketHandler, SIGNAL(disconnected(QWebSocketProtocol::CloseCode,QString)), this, SLOT(onWebSocketHandler_Disconnected(QWebSocketProtocol::CloseCode,QString)));
    disconnect(m_socketHandler, SIGNAL(binaryMessageReceived(QByteArray)), this, SLOT(onWebSocketHandler_BinaryMessageReceived(QByteArray)));

    delete m_socketHandler;
    if(m_serial->isOpen())
    {
        m_serial->close();
    }
    delete m_serial;
}

Repository &ArduinoU2Adapter::getRepository()
{
    return m_repository;
}


ArduinoU2Adapter &ArduinoU2Adapter::getInstance()
{
    static QScopedPointer<ArduinoU2Adapter> m_instance;
    if(m_instance.data() == nullptr)
    {
        m_instance.reset( new ArduinoU2Adapter() );
    }
    return *m_instance;
}

void ArduinoU2Adapter::openSerialPort(const QSerialPortInfo &info)
{
    m_serial->setPort(info);
    if (m_serial->open(QIODevice::ReadWrite))
    {
        qDebug() << "opened" << m_serial->portName();
        emit serialPortOpened();
    }
    else {
        qApp->quit();
    }
}

void ArduinoU2Adapter::onQSerialPort_ReadyRead()
{
    this->m_repository.m_serialPortReadBuffer.append(m_serial->readAll());
    bool ok = false;
    QtJson::parse(QString::fromUtf8(this->m_repository.m_serialPortReadBuffer), ok);
    if(ok)
    {
        this->processMessageFromSerialPort(QString::fromUtf8(this->m_repository.m_serialPortReadBuffer));
        this->m_repository.m_serialPortReadBuffer.clear();
    }
}

void ArduinoU2Adapter::processMessageFromSerialPort(QString message)
{
    bool parsed = false;
    QtJson::JsonObject parsedMessage = QtJson::parse(message, parsed).toMap();
    if(!parsed) return;

    m_repository.updateCurrentState(parsedMessage);
    this->sendStateToServer();
}

void ArduinoU2Adapter::sendStateToServer()
{
    QByteArray data = QtJson::serialize(m_repository.currentState());
    qDebug() << "send to server" << QString::fromUtf8(data);
    qDebug() << " ";
    m_socketHandler->sendBinaryMessage(data);
}

void ArduinoU2Adapter::onWebSocketHandler_BinaryMessageReceived(QByteArray message)
{
    QString messageString = QString::fromUtf8(message);
    bool parsed = false;
    QtJson::JsonObject parsedMessage = QtJson::parse(messageString, parsed).toMap();
    if(!parsed) return;

    QString target = parsedMessage["target"].toString();
    if(target.toLower() != "u2") return;

    QString action = parsedMessage["action"].toString();
    if(action == "getCurrentState")
    {
        this->sendStateToServer();
        return;
    }

    QString gcodesFrame = parsedMessage["frame"].toString();
    QtJson::JsonObject gcodesDetailedInfo = parsedMessage["detailedInfo"].toMap();

    QString gcodesFrameId = gcodesDetailedInfo["frameId"].toString();
    //int axisCount = gcodesDetailedInfo["axesCount"].toInt();
    QtJson::JsonObject axesArguments = gcodesDetailedInfo["axesArguments"].toMap();
    int feedrate = gcodesDetailedInfo["feedrate"].toInt();

    QtJson::JsonArray cmds = {};
    for(auto& axis : m_repository.m_axes)
    {
        if(!axesArguments.contains(axis.getId())) continue;

        int position = int(round(axesArguments[axis.getId()].toDouble() * 100));
        if(position == axis.getMotor().targetPos()) continue;

        if(axis.getMotor().isMoving())
        {
            qDebug() << "ArduinoU2Adapter::onWebSocketHandler_BinaryMessageReceived: motor is moving already. Command"
                     << messageString << "is dropped";
            continue;
        }

        cmds.append(axis.getMotor().prepareMotorCmd(position, feedrate));
    }


    for(auto cmd : cmds)
    {
        qDebug() << "execute" << QString::fromUtf8(QtJson::serialize(cmd));
        m_serial->write(QtJson::serialize(cmd));
    }

    this->sendStateToServer();
}

void ArduinoU2Adapter::onWebSocketHandler_Connected()
{
    qDebug() << "Web socket is connected";
}

void ArduinoU2Adapter::onWebSocketHandler_Disconnected(QWebSocketProtocol::CloseCode code, QString message)
{
    qDebug() << "Web socket disconnected with message" << message << "(code" << code << ")";
}
