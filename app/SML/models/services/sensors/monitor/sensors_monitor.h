#ifndef SENSORSMONITOR_H
#define SENSORSMONITOR_H

#include <QObject>
#include "models/types/sensor/sensor.h"

/**
 * @brief Класс Монитор состояния датчиков
 *
 * Предназначен для отслеживания состояния датчиков в режиме реального времени.
 */
class SensorsMonitor : public QObject
{
    Q_OBJECT
public:

    /**
     * @brief Конструктор класса
     * Связывает каждый датчик с функцией обработчкиком (onSensor_StateChanged).
     *
     * @param sensors список умных указателей на датчики, состояние которых требуется отслеживать
     * @param parent родительский объект в дереве объектов
     */
    explicit SensorsMonitor(QList<QSharedPointer<Sensor> > &sensors, QObject *parent = nullptr);

signals:

    /**
     * @brief Сигнал об изменении состояния датчика
     * @param uid уникальный идентификатор датчика
     * @param state обновленное состояние датчика
     */
    void stateChanged(QString uid, bool state);

protected slots:

    /**
     * @brief Испускает сигнал об изменении состояния датчика
     * @param uid уникальный идентификатор датчика
     * @param enable обновленное состояние датчика
     */
    void onSensor_StateChanged(QString uid, bool enable);
};

#endif // SENSORSMONITOR_H