#include "machine_tool.h"

MachineTool::MachineTool(QObject *parent) :
    QObject(parent),
    m_repository(new Repository(this)),
    m_adapterServer(new SMLServer(m_repository->m_port, this)),
    m_adaptersMonitor(new AdaptersMonitor(m_repository->m_u1Adapter.data(),
                                               m_repository->m_u2Adapter.data(),
                                               this)),
    m_pointsMonitor(new PointsMonitor(m_repository->m_pointsManager.data(), this)),
    m_sensorsMonitor(new SensorsMonitor(m_repository->m_sensors, this)),
    m_spindelsMonitor(new SpindelsMonitor(m_repository->m_spindels, this)),
    m_gcodesMonitor(new GCodesMonitor(m_repository->m_gcodesFilesManager.data(), this)),
    m_lastError(0)
{
    setupConnections();
    startAdapterServer();
}

MachineTool::~MachineTool()
{
    resetConnections();
}

MachineTool &MachineTool::getInstance()
{
    static QScopedPointer<MachineTool> m_instance;
    if(m_instance.data() == nullptr)
    {
        m_instance.reset( new MachineTool() );
    }
    return *m_instance;
}

Repository *MachineTool::getRepository()
{
    return m_repository.data();
}

void MachineTool::setupConnections()
{
    QObject::connect(m_adapterServer.data(), SIGNAL(u1Connected()), this, SLOT(onServer_U1Connected()));
    QObject::connect(m_adapterServer.data(), SIGNAL(u1Disconnected()), this, SLOT(onServer_U1Disconnected()));
    QObject::connect(m_adapterServer.data(), SIGNAL(u1StateChanged(QList<QVariant>,QList<QVariant>, int, int)),
                     this, SLOT(onServer_U1StateChanged(QList<QVariant>,QList<QVariant>, int, int)));
    QObject::connect(m_adapterServer.data(), SIGNAL(errorOccured(int)), this, SLOT(onServer_ErrorOccured(int)));

    QObject::connect(m_adaptersMonitor.data(), SIGNAL(u1AdapterConnectionStateChanged(bool)), this, SLOT(onAdaptersMonitor_U1AdapterConnectionStateChanged(bool)));
    QObject::connect(m_adaptersMonitor.data(), SIGNAL(u1AdapterWorkflowStateChanged(int)), this, SLOT(onAdaptersMonitor_U1AdapterWorkflowStateChanged(int)));

    QObject::connect(m_pointsMonitor.data(), SIGNAL(pointsUpdated()), this, SLOT(onPointsMonitor_PointsUpdated()));
    QObject::connect(m_sensorsMonitor.data(), SIGNAL(stateChanged(QString,bool)), this, SLOT(onSensorMonitor_StateChanged(QString,bool)));
    QObject::connect(m_spindelsMonitor.data(), SIGNAL(stateChanged(QString,bool,size_t)), this, SLOT(onSpindelsMonitor_StateChanged(QString,bool,size_t)));

    QObject::connect(m_gcodesMonitor.data(), SIGNAL(filePathUpdated(QString)), this, SLOT(onGCodesMonitor_FilePathUpdated(QString)));
    QObject::connect(m_gcodesMonitor.data(), SIGNAL(fileContentUpdated(QStringList)), this, SLOT(onGCodesMonitor_FileContentUpdated(QStringList)));
}

void MachineTool::resetConnections()
{
    QObject::disconnect(m_adapterServer.data(), SIGNAL(u1Connected()), this, SLOT(onServer_U1Connected()));
    QObject::disconnect(m_adapterServer.data(), SIGNAL(u1Disconnected()), this, SLOT(onServer_U1Disconnected()));
    QObject::disconnect(m_adapterServer.data(), SIGNAL(u1StateChanged(QList<QVariant>,QList<QVariant>, int, int)),
                     this, SLOT(onServer_U1StateChanged(QList<QVariant>,QList<QVariant>, int, int)));
    QObject::connect(m_adapterServer.data(), SIGNAL(errorOccured(int)), this, SLOT(onServer_ErrorOccured(int)));

    QObject::disconnect(m_adaptersMonitor.data(), SIGNAL(u1AdapterConnectionStateChanged(bool)), this, SLOT(onAdaptersMonitor_U1AdapterConnectionStateChanged(bool)));
    QObject::disconnect(m_adaptersMonitor.data(), SIGNAL(u1AdapterWorkflowStateChanged(int)), this, SLOT(onAdaptersMonitor_U1AdapterWorkflowStateChanged(int)));

    QObject::disconnect(m_pointsMonitor.data(), SIGNAL(pointsUpdated()), this, SLOT(onPointsMonitor_PointsUpdated()));
    QObject::disconnect(m_sensorsMonitor.data(), SIGNAL(stateChanged(QString,bool)), this, SLOT(onSensorMonitor_StateChanged(QString,bool)));
    QObject::disconnect(m_spindelsMonitor.data(), SIGNAL(stateChanged(QString,bool,size_t)), this, SLOT(onSpindelsMonitor_StateChanged(QString,bool,size_t)));

    QObject::disconnect(m_gcodesMonitor.data(), SIGNAL(filePathUpdated(QString)), this, SLOT(onGCodesMonitor_FilePathUpdated(QString)));
    QObject::disconnect(m_gcodesMonitor.data(), SIGNAL(fileContentUpdated(QStringList)), this, SLOT(onGCodesMonitor_FileContentUpdated(QStringList)));
}

void MachineTool::startAdapterServer()
{
    m_adapterServer->start();
}

void MachineTool::stopAdapterServer()
{
    m_adapterServer->stop();
}

QStringList MachineTool::getConnectedAdapters()
{
    return m_adapterServer->currentAdapters();
}

QString MachineTool::getAdapterServerPort()
{
    return QString::number(m_adapterServer->port());
}

int MachineTool::getLastError()
{
    return m_lastError;
}

void MachineTool::setLastError(int value)
{
    m_lastError = value;
    if(m_lastError != 0)
    {
        // вызов интерактора-обработчика
        emit errorOccured(m_lastError);
    }
}

void MachineTool::switchSpindelOn(QString index, size_t rotations)
{
    if(m_lastError != 0)
    {
        return;
    }

    SwitchSpindel swithcer(m_adapterServer.data(), index, true, rotations);
    swithcer.execute();
}

void MachineTool::switchSpindelOff(QString index)
{
    if(m_lastError != 0)
    {
        return;
    }

    SwitchSpindel switcher(m_adapterServer.data(), index, false);
    switcher.execute();
}

void MachineTool::onServer_U1Connected()
{
    m_repository->setU1ConnectState(true);
}


void MachineTool::onServer_U1Disconnected()
{
    m_repository->setU1ConnectState(false);
}

void MachineTool::onServer_U1StateChanged(QList<QVariant> sensors, QList<QVariant> devices, int workflowState, int lastError)
{
    try
    {
        m_repository->setU1Sensors(sensors);
        m_repository->setU1Devices(devices);
        m_repository->setU1WorkflowState(workflowState);
        if(lastError != 0)
        {
            setLastError(lastError);
        }
    }
    catch(SynchronizeStateException e)
    {
        qDebug() << "MachineTool::onServer_U1StateChanged:" << e.message();
        setLastError(-255);
    }
}

void MachineTool::onServer_ErrorOccured(int errorCode)
{
    setLastError(errorCode);
}

void MachineTool::onAdaptersMonitor_U1AdapterConnectionStateChanged(bool connected)
{
    if(connected)
    {
        emit u1Connected();
    }
    else
    {
        emit u1Disconnected();
    }
}

void MachineTool::onAdaptersMonitor_U1AdapterWorkflowStateChanged(int state)
{
    qDebug() << "Workflow state of u1 adapter is" << state << "now";
}

void MachineTool::onPointsMonitor_PointsUpdated()
{
    emit pointsUpdated();
}

void MachineTool::onSensorMonitor_StateChanged(QString sensorName, bool state)
{
    /*if(sensorName == "name")
    {
        do somtething
    }*/
    QColor led = QColor(SmlColors::white());
    if(state)
    {
        led = m_repository->getSensor(sensorName).getColor();
    }
    emit sensorStateChanged(sensorName, led);
}

void MachineTool::onSpindelsMonitor_StateChanged(QString index, bool state, size_t rotations)
{
    emit spindelStateChanged(index, state, rotations);
}

void MachineTool::onGCodesMonitor_FilePathUpdated(QString path)
{
    emit gcodesFilePathUpdated(path);
}

void MachineTool::onGCodesMonitor_FileContentUpdated(QStringList content)
{
    emit gcodesFileContentUpdated(content);
}