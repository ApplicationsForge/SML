#ifndef SMLKAMEDITORSERVER_H
#define SMLKAMEDITORSERVER_H

#include <QObject>
#include <QWebSocketServer>
#include <QWebSocket>

#include "models/structs.h"
#include "models/settingsmanager/settingsmanager.h"

#include "models/serverManager/smlkameditorserver/adapterscontainer/adapterscontainer.h"

class SMLKAMEditorServer : public QObject
{
    Q_OBJECT
public:
    explicit SMLKAMEditorServer(SettingsManager *settingsManager = nullptr, QObject *parent = nullptr);
    ~SMLKAMEditorServer();

protected:
    QWebSocketServer *m_server;
    qint16 m_port;
    AdaptersContainer m_adapters;
    bool m_debug;
signals:
    void newConnection();

    void u1Connected();
    void u1Disconnected();
    void u2Connected();
    void u2Disconnected();

    void byteMessageReceived(QByteArray message);

public slots:
    void start();
    void stop();
    void sendMessageToU1(QByteArray message);

    QStringList currentAdapters();
    size_t port() const;

protected slots:
    void setup(SettingsManager* sm);
    void onServerClosed();
    void onNewConnection();
    void onTextMessage(QString message);
    void onBinaryMessage(QByteArray message);
    void socketDisconnected();
    void registerConnection(QWebSocket *connection, int type);
};

#endif // SMLKAMEDITORSERVER_H