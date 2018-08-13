#ifndef CONNECTIONSMONITOR_H
#define CONNECTIONSMONITOR_H

#include <QObject>
#include "models/types/connection/connection.h"

class ConnectionsMonitor : public QObject
{
    Q_OBJECT
public:
    explicit ConnectionsMonitor(Connection* u1, Connection* u2, QObject *parent = nullptr);

protected:
    Connection* m_u1;
    Connection* m_u2;

signals:
    void u1Connected();
    void u1Disconnected();

    void u2Connected();
    void u2Disconnected();

protected slots:
    void onU1_ConnectionStateChanged(bool state);
    void onU2_ConnectionStateChanged(bool state);
};

#endif // CONNECTIONSMONITOR_H