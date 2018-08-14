#ifndef SENSORSBUFFER_H
#define SENSORSBUFFER_H

#include <QObject>

#include "models/types/structs.h"
#include "models/types/exceptions/synchronizestateexception.h"

class SensorsBuffer : public QObject
{
    Q_OBJECT
public:
    explicit SensorsBuffer(size_t bufferSize = 16, QObject *parent = nullptr);

    /*!
     * \brief Заполняет буфер нулями
     * \param size - необходимый размер буфера
     */
    void resetBuffer(size_t size);

    /*!
     * \brief Проверяет изменилось ли состояние порта
     * \param currentState - байт, содержащий текущее состояние порта
     * \param newState - байт, содержащий новое значение состояния порта
     * \return true, если состояние изменилось, false - иначе
     */
    bool isPortStateChanged(byte currentState, byte newState);

    /*!
     * \brief Обновляет состояние буфера обмена данными
     * \param value - новое значение буфера
     */
    void updateBuffer(byte_array value);

    /*!
     * \brief Проверяет активен ли вход
     * \param plateName - имя платы, к которой подключен датчик (Доступные имена: "u1", "u2", "portal")
     * \param portNumber - номер порта, к которому подключен датчик
     * \param inputNumber - номер входа, к которому подключен датчик
     * \return true, если есть напряжение на входе, false - иначе
     */
    bool getInputState(QString plateName, unsigned int portNumber, unsigned int inputNumber) const;

protected:
    /*!
     * \brief buffer - содержит всю информацию о состояниях датчиков станка
     * Размер буфера должен составлять не менее 2 байт.
     * Каждый байт представляет собой состояние какого-либо порта.
     * К одному порту может быть подключено до 8 датчиков.
     * Таким образом, максимальное число поддерживаемых датчиков равно buffer_size * 8.
     * Описание структуры буфера
     * buffer[0] - датчики 0 порта портальной платы (1 байт)
     * buffer[1] - датчики 1 порта портальной платы (1 байт)
     * buffer[2] - датчики 2 порта портальной платы (1 байт)
     * buffer[3] - датчики 3 порта портальной платы (1 байт)
     * buffer[4] - датчики 4 порта портальной платы (1 байт)
     * buffer[5] - датчики 5 порта портальной платы (1 байт)
     * buffer[6] - датчики 6 порта портальной платы (1 байт)
     * buffer[7] - датчики 7 порта портальной платы (1 байт)
     * buffer[8] - датчики 0 порта U1 (1 байт)
     * buffer[9] - датчики 1 порта U1 (1 байт)
     * buffer[10] - датчики 2 порта U1 (1 байт)
     * buffer[11] - датчики 3 порта U1 (1 байт)
     * buffer[12] - датчики 4 порта U1 (1 байт)
     * buffer[13] - датчики 5 порта U1 (1 байт)
     * buffer[14] - датчики 6 порта U1 (1 байт)
     * buffer[15] - датчики 7 порта U1 (1 байт)
     * Если размер буфера будет меньше станартного размера (16 байт),
     * то структура выше определяет, какие порты портальной платы и контроллера U1 не будут задействованы.
     * Если размер буфера будет больше, то данные с 17 байта и дальше  будут игнорироваться.
     */
    byte_array m_buffer;

    /*!
     * \brief Функция для стандартной проверки состояния датчика
     * Проверяет только наличие напряжения на входе. Т.е. true - напряжение есть. false - напряжения нет.
     * \warning Не определяет включен датчик или выключен!
     * Применима, если номер входа датчика = номеру бита в байте, кодирующем состояние группы датчиков.
     * Нумерация в байте порта должна идти с младшего разряда и, начинаться, с нуля.
     * Пример:
     * 1111[1]11
     * В данном случае, датчик, состояние которого выделено [], подключен к 1 порту и 2 входу портальной платы.
     * \param inputNumber - номер входа, к которому подключен датчик
     * \param portState - состояние порта, к которому подключен датчик
     * \return есть ли напряжение на входе
     */
    bool standardInputStateCheck(size_t inputNumber, byte portState) const;

    /*!
     * \brief Проверяет состояние датчика Портальной платы
     * Проверяет только наличие напряжения на входе. Т.е. true - напряжение есть. false - напряжения нет.
     * \warning Не определяет включен датчик или выключен!
     * \param portNumber - номер порта, к которому подключен датчик
     * \param inputNumber - номер входа, к которому подключен датчик
     * \return true, если есть напряжение на входе, false - иначе
     */
    bool checkPortalSensorState(unsigned int portNumber, unsigned int inputNumber) const;

    /*!
     * \brief Проверяет состояние датчика, подключенного к U1
     * Проверяет только наличие напряжения на входе. Т.е. true - напряжение есть. false - напряжения нет.
     * \warning Не определяет включен датчик или выключен!
     * \param portNumber - номер порта, к которому подключен датчик
     * \param inputNumber - номер входа, к которому подключен датчик
     * \return true, если есть напряжение на входе, false - иначе
     */
    bool checkU1SensorState(unsigned int portNumber, unsigned int inputNumber) const;

signals:

public slots:
};

#endif // SENSORSBUFFER_H
