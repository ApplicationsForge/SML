#include "u2_serial_adapter.h"

U2SerialAdapter::U2SerialAdapter(QObject *parent) :
    QObject(parent),
    m_settingsManager(SettingsManager()),
    m_socketHandler(new WebSocketHandler(m_settingsManager, this)),
    m_currentState(U2State(0, 0))
{
    this->loadSettings();

    QObject::connect(m_socketHandler, SIGNAL(connected()), this, SLOT(onWebSocketHandler_Connected()));
    QObject::connect(m_socketHandler, SIGNAL(disconnected(QWebSocketProtocol::CloseCode,QString)), this, SLOT(onWebSocketHandler_Disconnected(QWebSocketProtocol::CloseCode,QString)));
    QObject::connect(m_socketHandler, SIGNAL(binaryMessageReceived(QByteArray)), this, SLOT(onWebSocketHandler_BinaryMessageReceived(QByteArray)));
}

U2SerialAdapter::~U2SerialAdapter()
{
    QObject::disconnect(m_socketHandler, SIGNAL(connected()), this, SLOT(onWebSocketHandler_Connected()));
    QObject::disconnect(m_socketHandler, SIGNAL(disconnected(QWebSocketProtocol::CloseCode,QString)), this, SLOT(onWebSocketHandler_Disconnected(QWebSocketProtocol::CloseCode,QString)));
    QObject::disconnect(m_socketHandler, SIGNAL(binaryMessageReceived(QByteArray)), this, SLOT(onWebSocketHandler_BinaryMessageReceived(QByteArray)));

    delete m_socketHandler;
}

void U2SerialAdapter::loadSettings()
{
    try
    {
        m_currentState.addAxisPosition("X", 0.0);
        m_currentState.addAxisPosition("Y", 0.0);
        m_currentState.addAxisPosition("Z", 0.0);
        m_currentState.addAxisPosition("A", 0.0);
        m_currentState.addAxisPosition("B", 0.0);
        m_currentState.addAxisPosition("C", 0.0);
        m_currentState.addAxisPosition("U", 0.0);
        m_currentState.addAxisPosition("V", 0.0);
        m_currentState.addAxisPosition("W", 0.0);
        this->printState(m_currentState);
    }
    catch(std::invalid_argument e)
    {
        qDebug() << e.what();
    }
}

void U2SerialAdapter::printState(U2State state)
{
    qDebug() << "State";
    qDebug() << "AxisesCount:" << state.getAxesCount();
    qDebug() << "Positions:";
    QStringList axisesKeys = state.getAxisesKeys();
    for(auto key : axisesKeys)
    {
        qDebug() << key << ":" << state.getAxisPosition(key);
    }
    qDebug() << "WorkflowState:" << state.getWorkflowState();
    qDebug() << "LastError: " << state.getLastError();
    qDebug() << "---------" ;
}

void U2SerialAdapter::sendCurrentStateToServer(U2State state)
{
    QtJson::JsonObject message = {};
    QtJson::JsonObject u2State = {};
    u2State["lastError"] = state.getLastError();
    u2State["workflowState"] = state.getWorkflowState();
    u2State["axesCount"] = state.getAxesCount();
    QtJson::JsonArray axes = {};
    QStringList axisesKeys = state.getAxisesKeys();
    for(auto key : axisesKeys)
    {
        QtJson::JsonObject axis = {};
        axis["id"] = key;
        axis["position"] = state.getAxisPosition(key);
        axes.append(axis);
    }
    u2State["axes"] = axes;
    message["u2State"] = u2State;

    QByteArray data = QtJson::serialize(message);
    m_socketHandler->sendBinaryMessage(data);
}

void U2SerialAdapter::onWebSocketHandler_BinaryMessageReceived(QByteArray message)
{
    this->printState(m_currentState);
    QString messageString = QString::fromUtf8(message);
    bool ok = false;
    QtJson::JsonObject parsedMessage = QtJson::parse(messageString, ok).toMap();
    if(ok)
    {
        QString target = parsedMessage["target"].toString();
        if(target.toLower() == "u2")
        {
            QtJson::JsonObject detailedInfo = parsedMessage["detailedInfo"].toMap();
            QtJson::JsonObject axesArgumentsRaw = detailedInfo["axesArguments"].toMap();
            QMap<QString, double> axesArgumentsMap = {};
            for(auto axisId : axesArgumentsRaw.keys())
            {
                bool ok = false;
                double value = axesArgumentsRaw[axisId].toDouble(&ok);

                if(ok)
                {
                    axesArgumentsMap.insert(axisId, value);
                }
            }

            qDebug() << "start processing";
            m_currentState.setWorkflowState(1);

            // calculate increments
            QStringList axisArgumentsKeys = axesArgumentsMap.keys();
            int stepsCount = 100;
            QMap<QString, double> stepIncrements = {};
            for(auto key : axisArgumentsKeys)
            {
                double targetValue = axesArgumentsMap[key];
                double diff = (targetValue - m_currentState.getAxisPosition(key));
                double stepByAxis = diff / stepsCount;
                stepIncrements.insert(key, stepByAxis);
            }

            // simulate move
            for(int i = 0; i < stepsCount; i++)
            {
                for(auto key : axisArgumentsKeys)
                {
                    double newAxisPosition = m_currentState.getAxisPosition(key) + stepIncrements[key];
                    m_currentState.setAxisPosition(key, newAxisPosition);
                }

                qDebug() << "send state" << i;
                this->sendCurrentStateToServer(m_currentState);

                // wait
                U2SerialAdapter::printState(m_currentState);
                for(int i = 0; i < 100000; i++);
            }
            qDebug() << "finish processing";
            m_currentState.setWorkflowState(0);
            this->sendCurrentStateToServer(m_currentState);
        }
        else
        {
            qDebug() << "Message ignored";
        }
    }
}

void U2SerialAdapter::sendTestPackageToServer()
{
    sendCurrentStateToServer(m_currentState);
}

void U2SerialAdapter::onWebSocketHandler_Connected()
{
    qDebug() << "Web socket is connected";
    sendTestPackageToServer();
}

void U2SerialAdapter::onWebSocketHandler_Disconnected(QWebSocketProtocol::CloseCode code, QString message)
{
    qDebug() << "Web socket disconnected with message" << message << "(code" << code << ")";
}
