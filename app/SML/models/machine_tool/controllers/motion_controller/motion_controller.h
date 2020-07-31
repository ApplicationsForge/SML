#ifndef MOTIONCONTROLLER_H
#define MOTIONCONTROLLER_H

#include <QObject>
#include "libs/json_parser/json.h"

#include "../base_controller.h"

#include "./axis/axis.h"
#include "./axis/point.h"

/**
 * @brief Контроллер двигаталей.
 */
class MotionController : public BaseController
{
    Q_OBJECT
public:
    /**
     * @brief Конструктор класса.
     * @param parent - родительский объект
     */
    explicit MotionController(QObject *parent = nullptr);

    /**
     * @brief Деструктор класса.
     */
    ~MotionController() override;

    /**
     * @brief Возвращает текущее положение станка относительно базы.
     * @return Текущее положение станка в абсолютных координатах.
     */
    Point currentPos();

    bool axisExists(AxisId id);
    void addAxis(AxisId id, double initialPosition);
    void removeAxis(AxisId id);
    void clearAxes();

    //void moveTo(Point absPos);
    //void moveOffset(Point relPos);

    //void executeChunk(QString gcode);

    //void stopMoving();

private:
    /// Доступные оси станка.
    QSet<Axis*> m_axes;
    QList<QMetaObject::Connection> m_slotsInfo;

    /**
     * @brief Устанавливает текущее положение станка относительно базы.
     * @param absPos - обновленное положение станка относительно базы.
     */
    void setCurrentPos(Point absPos);

    /**
     * @brief Обработчик сообщения от адаптера в виде массива байт.
     * @param message - принятое сообщение от адаптера
     */
    void parseBinaryMessage(QByteArray message) override;

    /**
     * @brief Обработчик сообщения от адаптера в виде текста.
     * @param message - принятое сообщение от адаптера
     */
    void parseTextMessage(QString message) override;

    Axis* findById(AxisId id);

signals:
    void axesListChanged();
};

#endif // MOTIONCONTROLLER_H