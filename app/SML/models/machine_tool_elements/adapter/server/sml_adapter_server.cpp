#include "sml_adapter_server.h"

SMLAdapterServer::SMLAdapterServer(qint16 port, QObject *parent) :
    QObject(parent),
    m_server(QStringLiteral("Echo Server"), QWebSocketServer::NonSecureMode, this),
    m_port(port),
    m_debug(false)
{
    this->setupConnections();
}

SMLAdapterServer::~SMLAdapterServer()
{
    this->stop();
    this->resetConnections();

    for(auto socket : m_u1Connections)
    {
        if(socket)
        {
            if(socket->isValid())
            {
                QObject::disconnect(socket, SIGNAL(textMessageReceived(QString)), this, SLOT(onQWebSocket_TextMessageReceived(QString)));
                QObject::disconnect(socket, SIGNAL(binaryMessageReceived(QByteArray)), this, SLOT(onQWebSocket_BinaryMessageReceived(QByteArray)));
                QObject::disconnect(socket, SIGNAL(disconnected()), this, SLOT(onQWebSocket_Disconnected()));
            }
        }
    }
    qDeleteAll(m_u1Connections.begin(), m_u1Connections.end());

    for(auto socket : m_u2Connections)
    {
        if(socket)
        {
            if(socket->isValid())
            {
                QObject::disconnect(socket, SIGNAL(textMessageReceived(QString)), this, SLOT(onQWebSocket_TextMessageReceived(QString)));
                QObject::disconnect(socket, SIGNAL(binaryMessageReceived(QByteArray)), this, SLOT(onQWebSocket_BinaryMessageReceived(QByteArray)));
                QObject::disconnect(socket, SIGNAL(disconnected()), this, SLOT(onQWebSocket_Disconnected()));
            }
        }
    }
    qDeleteAll(m_u2Connections.begin(), m_u2Connections.end());

    for(auto socket : m_unregistered)
    {
        if(socket)
        {
            if(socket->isValid())
            {
                QObject::disconnect(socket, SIGNAL(textMessageReceived(QString)), this, SLOT(onQWebSocket_TextMessageReceived(QString)));
                QObject::disconnect(socket, SIGNAL(binaryMessageReceived(QByteArray)), this, SLOT(onQWebSocket_BinaryMessageReceived(QByteArray)));
                QObject::disconnect(socket, SIGNAL(disconnected()), this, SLOT(onQWebSocket_Disconnected()));
            }
        }
    }
    qDeleteAll(m_unregistered.begin(), m_unregistered.end());

}

void SMLAdapterServer::setupConnections()
{
    QObject::connect(&m_server, SIGNAL(newConnection()), this, SLOT(onQWebSocketServer_NewConnection()));
    QObject::connect(&m_server, SIGNAL(closed()), this, SLOT(onQWebSocketServer_Closed()));
}

void SMLAdapterServer::resetConnections()
{
    QObject::disconnect(&m_server, SIGNAL(newConnection()), this, SLOT(onQWebSocketServer_NewConnection()));
    QObject::disconnect(&m_server, SIGNAL(closed()), this, SLOT(onQWebSocketServer_Closed()));
}

void SMLAdapterServer::start()
{
    if(m_port != 0)
    {
        if (m_server.listen(QHostAddress::Any, m_port))
        {
            qDebug() << "Hello! SML Server is listening on port" << m_port;
            if (m_debug)
            {
                qDebug() << m_server.error();
                qDebug() << m_server.errorString();
            }
        }
    }
}

void SMLAdapterServer::stop()
{
    qDebug() << "stop server";
    if(m_server.isListening())
    {
        m_server.close();
    }
    qDebug() << "Server successfully stopped. Good Bye!";
}

void SMLAdapterServer::onQWebSocketServer_NewConnection()
{
    QWebSocket* pSocket = m_server.nextPendingConnection();

    QObject::connect(pSocket, SIGNAL(textMessageReceived(QString)), this, SLOT(onQWebSocket_TextMessageReceived(QString)));
    QObject::connect(pSocket, SIGNAL(binaryMessageReceived(QByteArray)), this, SLOT(onQWebSocket_BinaryMessageReceived(QByteArray)));
    QObject::connect(pSocket, SIGNAL(disconnected()), this, SLOT(onQWebSocket_Disconnected()));

    m_unregistered.push_back(pSocket);

    if(m_debug)
    {
        qDebug() << "SMLAdapterServer::onQWebSocketServer_NewConnection: Socket connected:" << pSocket->peerAddress() <<  pSocket->peerName() << pSocket->origin();
    }

}

void SMLAdapterServer::sendMessageToU1(QByteArray message)
{
    for(auto socket : m_u1Connections)
    {
        if(socket)
        {
            if(socket->isValid())
            {
                socket->sendBinaryMessage(message);
            }
            else
            {
                throw SynchronizeStateException("WebSocket is invalid");
            }
        }
        else
        {
            throw SynchronizeStateException("WebSocket is null");
        }
    }
}

void SMLAdapterServer::sendMessageToU2(QByteArray message)
{
    for(auto socket : m_u2Connections)
    {
        if(socket)
        {
            if(socket->isValid())
            {
                socket->sendBinaryMessage(message);
            }
            else
            {
                throw SynchronizeStateException("WebSocket is invalid");
            }
        }
        else
        {
            throw SynchronizeStateException("WebSocket is null");
        }
    }
}

void SMLAdapterServer::sendMessage(QByteArray message)
{
    this->sendMessageToU1(message);
    this->sendMessageToU2(message);
}

void SMLAdapterServer::onQWebSocketServer_Closed()
{
    if(m_debug)
    {
        qDebug() << "SMLAdapterServer::onQWebSocketServer_Closed: Server Closed";
        qDebug() << m_server.error();
        qDebug() << m_server.errorString();
    }
    emit this->u1Disconnected();
    emit this->u2Disconnected();
}

void SMLAdapterServer::onQWebSocket_TextMessageReceived(QString message)
{
    if (m_debug)
    {
        qDebug() << "SMLAdapterServer::onQWebSocket_TextMessageReceived:" << message;
    }

    QWebSocket* pSender = qobject_cast<QWebSocket *>(sender());
    if (!pSender) return;

    if(message == "@SML-U1Adapter@")
    {
        try
        {
            this->registerClient(pSender, SMLAdapterServer::U1Adapter);
            pSender->sendTextMessage("Registered!");
        }
        catch(SynchronizeStateException e)
        {
            qDebug() << e.message();
            emit this->errorOccurred(ERROR_CODE::SYNC_STATE_ERROR);
        }
    }
    else
    {
        if(message == "@SML-U2Adapter@")
        {
            try
            {
                this->registerClient(pSender, SMLAdapterServer::U2Adapter);
                pSender->sendTextMessage("Registered!");
            }
            catch(SynchronizeStateException e)
            {
                qDebug() << e.message();
                emit this->errorOccurred(ERROR_CODE::SYNC_STATE_ERROR);
            }
        }
        else
        {
            pSender->sendTextMessage("Connection aborted");
            pSender->close();
        }
    }
}

void SMLAdapterServer::onQWebSocket_BinaryMessageReceived(QByteArray message)
{
    if(m_debug)
    {
        qDebug() << "SMLAdapterServer::onQWebSocket_BinaryMessageReceived:" << QString::fromUtf8(message);
    }

    QWebSocket* pSender = qobject_cast<QWebSocket *>(sender());
    if (!pSender) return;

    if(m_u1Connections.contains(pSender))
    {
        try
        {
            U1State u1 = SMLAdapterServer::parseU1BinaryMessage(message);
            emit this->u1StateChanged(u1.sensors, u1.devices, u1.workflowState, ERROR_CODE(u1.errorCode));
        }
        catch(SynchronizeStateException e)
        {
            qDebug() << e.message();
            emit this->errorOccurred(ERROR_CODE::SYNC_STATE_ERROR);
        }
        return;
    }

    if(m_u2Connections.contains(pSender))
    {
        try
        {
            U2State u2 = SMLAdapterServer::parseU2BinaryMessage(message);
            emit this->u2StateChanged(u2.positions, u2.workflowState, ERROR_CODE(u2.errorCode));
        }
        catch(SynchronizeStateException e)
        {
            qDebug() << e.message();
            emit this->errorOccurred(ERROR_CODE::SYNC_STATE_ERROR);
        }
        return;
    }
}

U1State SMLAdapterServer::parseU1BinaryMessage(QByteArray message)
{
    bool parsed = false;
    QString json = QString::fromUtf8(message);
    QtJson::JsonObject result = QtJson::parse(json, parsed).toMap();
    if(!parsed)
    {
        throw SynchronizeStateException("An error is occurred during parsing json" + QString::fromUtf8(message));
    }

    QtJson::JsonObject u1State = result["u1_state"].toMap();
    if(u1State.isEmpty())
    {
        throw SynchronizeStateException("empty u1 message");
    }

    U1State u1;
    u1.sensors = u1State["sensors_state"].toList();
    u1.devices = u1State["devices_state"].toList();
    u1.errorCode = u1State["last_error"].toInt();
    u1.workflowState = 0;
    return u1;
}

U2State SMLAdapterServer::parseU2BinaryMessage(QByteArray message)
{
    bool parsed = false;
    QString json = QString::fromUtf8(message);
    QtJson::JsonObject result = QtJson::parse(json, parsed).toMap();
    if(!parsed)
    {
        throw SynchronizeStateException("an error is occurred during parsing json" + QString::fromUtf8(message));
    }

    QtJson::JsonObject u2State = result["u2State"].toMap();
    if(u2State.isEmpty())
    {
        throw SynchronizeStateException("empty u2 message");
    }

    U2State u2;
    u2.errorCode = u2State["lastError"].toInt();
    u2.workflowState = u2State["workflowState"].toUInt();

    QtJson::JsonArray axises = u2State["axes"].toList();
    for(auto axis : axises)
    {
        QtJson::JsonObject axisObject = axis.toMap();
        QString id = axisObject["id"].toString();
        double value = axisObject["position"].toDouble();
        u2.positions.insert(id, value);
    }

    return u2;
}

void SMLAdapterServer::onQWebSocket_Disconnected()
{
    QWebSocket* pSender = qobject_cast<QWebSocket *>(sender());
    if (!pSender) return;

    if (m_debug)
    {
        qDebug() << "SMLAdapterServer::onQWebSocket_Disconnected:" << pSender;
    }

    if(m_u1Connections.contains(pSender))
    {
        m_u1Connections.removeAll(pSender);
        emit this->u1Disconnected();
    }
    else
    {
        if(m_u2Connections.contains(pSender))
        {
            m_u2Connections.removeAll(pSender);
            emit this->u2Disconnected();
        }
        else
        {
            m_unregistered.removeAll(pSender);
        }
    }

    pSender->deleteLater();
}

void SMLAdapterServer::registerClient(QWebSocket* client, int type)
{
    if (client == nullptr)
        throw SynchronizeStateException("Try to Register invalid socket");

    switch (type) {
    case SMLAdapterServer::U1Adapter:
        m_u1Connections.push_back(client);
        m_unregistered.removeAll(client);
        if(m_debug)
        {
            qDebug() << "SMLServer::registerClient: U1Adapter registered =" << client;
        }
        emit this->u1Connected();
        break;
    case SMLAdapterServer::U2Adapter:
        m_u2Connections.push_back(client);
        m_unregistered.removeAll(client);
        if(m_debug)
        {
            qDebug() << "SMLServer::registerClient: U2Adapter registered =" << client;
        }
        emit this->u2Connected();
        break;
    default:
        break;
    }

    if(m_debug)
    {
        qDebug() << "u1";
        for(auto socket : m_u1Connections)
        {
            qDebug() << socket;
        }

        qDebug() << "unregistered";
        for(auto socket : m_unregistered)
        {
            qDebug() << socket;
        }
    }
}

QStringList SMLAdapterServer::currentAdapters()
{
    QStringList settings;

    for(auto socket : m_u1Connections)
    {
        QString localPort = QString::number(socket->localPort());
        QString localAddress = socket->localAddress().toString();

        QString adapterSettingsString =
                QString("[U1Adapter] ") +
                "on local port [" + localPort + "] " +
                "with local address [" + localAddress + "]";
        settings.push_back(adapterSettingsString);
    }

    for(auto socket : m_u2Connections)
    {
        QString localPort = QString::number(socket->localPort());
        QString localAddress = socket->localAddress().toString();

        QString adapterSettingsString =
                QString("[U2Adapter] ") +
                "on local port [" + localPort + "] " +
                "with local address [" + localAddress + "]";
        settings.push_back(adapterSettingsString);
    }

    for(auto socket : m_unregistered)
    {
        QString localPort = QString::number(socket->localPort());
        QString localAddress = socket->localAddress().toString();

        QString adapterSettingsString =
                QString("Name [Unregistered] ") +
                "on local port [" + localPort + "] " +
                "with local address [" + localAddress + "]";
        settings.push_back(adapterSettingsString);
    }

    return settings;
}

size_t SMLAdapterServer::port() const
{
    return size_t (m_port);
}
