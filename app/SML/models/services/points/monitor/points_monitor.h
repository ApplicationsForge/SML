#ifndef POINTSMONITOR_H
#define POINTSMONITOR_H

#include <QObject>

#include "models/repository/repository.h"

/**
 * @brief Класс Монитор состояния точек
 *
 * Предназначен для отслеживания состояния точек в режиме реального времени.
 */
class PointsMonitor : public QObject
{
    Q_OBJECT
public:

    /**
     * @brief Конструктор класса
     * Связывает менеджер точек с функцией обработчиком (onPointsUpdated).
     *
     * @param manager указатель на менеджер точек, управляющий точками, которые необходимо отслеживать
     * @param parent родительский объект в дереве объектов
     */
    explicit PointsMonitor(Repository &repo, QObject *parent = nullptr);

signals:

    /**
     * @brief Сигнал об изменении состояния точек
     */
    void pointsUpdated();

protected slots:

    /**
     * @brief Испускает сигнал об изменении состояния точек
     */
    void onRepository_PointsUpdated();
};

#endif // POINTSMONITOR_H
