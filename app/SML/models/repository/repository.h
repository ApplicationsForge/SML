#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <QObject>

#include "models/types/axis/axis.h"
#include "models/types/device/spindel.h"
#include "models/types/device/supportdevice.h"
#include "models/types/sensor/sensor.h"
#include "models/types/sensor/sensorsbuffer.h"
#include "models/types/settingsmanager/settingsmanager.h"
#include "models/types/point/point.h"
#include "models/types/connection/connection.h"
#include "models/types/gcodes/filesreader/filesreader.h"

class MachineTool;
class Repository : public QObject
{
    Q_OBJECT
public:
    explicit Repository(QObject *parent = nullptr);

    void setU1Connected(bool connected);
    void setU1Sensors(QList<QVariant> sensors);
    void setU1Devices(QList<QVariant> devices);
    void setU1Error(int code);

    Device& findDevice(size_t index);

    Sensor* findSensor(QString name);
    QStringList getSensorNames();
    QMap<QString, QString> getSensorSettings(QString name);

    /// Устанавливает текущее значение G-Codes
    void setGCodes(const QString &data);
    /// Возвращает текущее значение G-Codes
    QString getGCodesProgram();

    /// Возвращает содержимое файла G-Codes
    QString getGCodesFileContent();

    /// Возвращает текущие координаты станка
    QList<Point> getMachineToolCoordinates();

    /// Вовращает значения настроей подключенных датчиков
    QStringList getSensorsSettings();

    /// Возвращает список всех подключенных устройств
    QStringList getAllDevicesNames();

    /// Возвращает настройки подключенных устройств
    QList<QStringList> getAllDevicesSettings();

    /// Возвращает список устройств, необходимых для отображения в Наладке
    QStringList getOnScreenDevicesNames();

    /// Возвращает список состояний подулюченных устройств
    /// true = устройство включено, false = устройство выключено
    QList<bool> getOnScreenDevicesStates();

    /// Возвращает названия активных осей координат
    QStringList getAxisesNames();

    /// Возвращает настройки активных осей координат
    QStringList getAxisesSettings();

    /// Возвращает нащвания подключенных опций
    QStringList getOptionsNames();

    QSharedPointer<Point> findPoint(size_t idx);

    /// Возвращает список точек
    QList<QStringList> getPoints();

    /// Возвращает координаты точки по ее индексу
    QStringList getPoint(unsigned int number);

    /// Возвращает путь до открытого файла
    QString getFilePath(QString type = "gcodes");

    /// Включает или отключает контроль габаритов
    void setSoftLimitsMode(bool enable);

    /// Возвращает список текущих подключений
    QStringList getCurrentConnections();

    /// Возвращает порт, на котором сервер принимает подключения
    QString getServerPort();

    /// Возвращает размер буфера датчиков
    QString getSensorsBufferSize();

    /// Возвращает размер буфера устройств
    QString getDevicesBufferSize();

    QList<Spindel *> getSpindels();

protected:
    /// Менеджер настроек
    QScopedPointer<SettingsManager> m_settingsManager;

    /// Сервер
    size_t m_port;

    /// Подключения
    QScopedPointer<Connection> m_u1Connection;
    QScopedPointer<Connection> m_u2Connection;

    /// Датчики
    QList< QSharedPointer<Sensor> > m_sensors;
    SensorsBuffer m_sensorsBuffer;

    /// Устройства
    QList< QSharedPointer<Spindel> > m_spindels;
    QList< QSharedPointer<SupportDevice> > m_supportDevices;

    /// Оси
    QList< QSharedPointer<Axis> > m_axises;
    Point m_zeroCoordinates;
    Point m_parkCoordinates;

    /// G-Codes
    QString m_gcodes;

    QString m_gCodesFilePath;
    QString m_gCodesFileContent;

    /// Точки
    QList< QSharedPointer<Point> > m_points;

    void loadSettigs();
    void loadServerSettings();
    void loadSensorsSettings();
    void loadDevicesSettings();
    void loadAxisesSettings();

    Point getCurrentCoordinatesFromBase();
    Point getCurrentCoordinatesFromZero();

    friend class MachineTool;

signals:

public slots:
};

#endif // REPOSITORY_H
