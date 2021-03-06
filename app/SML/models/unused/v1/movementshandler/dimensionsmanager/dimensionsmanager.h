#ifndef DIMENSIONSMANAGER_H
#define DIMENSIONSMANAGER_H
#include "models/machinetool/pointsmanager/point/point.h"

/**
 * @brief Класс "Контроллер Габаритов"
 *
 * Содержит число осей станка и параметр типа bool, отвечающий за включение/выключение контроля габаритов
 */
class DimensionsManager
{
private:
    /// число осей станка
    int axisesCount;

    /// активен ли контроль габаритов
    bool isDimensionControlEnable;
public:
    /*!
     * \brief Позволяет создать экземпляр класса " Контроллер габаритов"
     * \param _axisesCount число осей станка
     * \param _isDimensionControlEnable активен ли контроль габаритов (по умолчанию false)
     */
    DimensionsManager(int _axisesCount = 3, bool _isDimensionControlEnable = false);
    /*!
     * \brief Проверяет корректность перемещения
     * \param axisesLength длина по каждой из осей станка
     * \param newCoordinates координаты, в которые необходимо перейти
     * \return корректно ли перемещение
     */
    bool isMovementCorrect(Point &axisesLength, Point &newCoordinates);

    /*!
     * \brief Проверяет выходим ли мы за пределы какой-либо оси
     * \param axisesLength длина по каждой из осей станка
     * \param newCoordinates координаты, в которые нужно перейти (относительно базы)
     * \return корректно ли новые координаты
     */
    bool checkAxisesCoordinates(Point &axisesLength, Point &newCoordinates);

    /*!
     * \brief getIsDimensionControlEnable
     * \return  true - контроль габаритов активен, false - иначе
     */
    bool getIsDimensionControlEnable() const;

    /*!
     * \brief setIsDimensionControlEnable
     * \param value активен ли контроль габритов true - активен, false - иначе
     */
    void setIsDimensionControlEnable(bool value);
};

#endif // DIMENSIONSMANAGER_H
