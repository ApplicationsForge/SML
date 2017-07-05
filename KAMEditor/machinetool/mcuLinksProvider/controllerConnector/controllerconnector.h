#ifndef CONTROLLERCONNECTOR_H
#define CONTROLLERCONNECTOR_H
#include "libusb/include/libusb.h"

#include <string>

#include <QMainWindow>
#include <QSerialPort>
#include <QDebug>

#include "machinetool/structs.h"

/*!
 * \brief Класс "Модуль взаимодействия с контроллером"
 * \warning Является синглтоном
 * \brief Класс получает данные из буфера обмена данными и следит за корректностью разрядности;
 */
class ControllerConnector : QObject
{
private:
    QSerialPort *serialPort;

    ControllerConnector();
public:
    //! Возвращает экземпляр класса "Модуль взаимосвязи с  контроллером"
    static ControllerConnector& Instance();

    ~ControllerConnector();
public slots:

    //! Отправляет данные на контроллер
    void send();
    void recieved();
};

#endif // CONTROLLERCONNECTOR_H
