#include "movementshandler.h"

MovementsHandler::MovementsHandler(unsigned int _axisesCount, double _step, double _velocity) :
    currentCoordinates(_axisesCount),
    currentCoordinatesFromBase(_axisesCount),
    parkCoordinates(_axisesCount),
    step(_step),
    velocity(_velocity),
    dimensionsManager(_axisesCount)
{
    addAxises(_axisesCount);
    axisesLength = getAxisesLength();
}

void MovementsHandler::addAxises(const unsigned int &count)
{
    unsigned int axisCount = count;
    if(axisCount > 11)
    {
       axisCount = 11;
    }
    axises.reserve(axisCount);
    for(unsigned int i = 0; i < axisCount; i++)
    {
        std::string axisName = axisesNames.getNameByKey(i);
        Axis* newAxis = new Axis(axisName);
        axises.push_back(std::shared_ptr<Axis>(newAxis));
    }
    setupAxises();
}

void MovementsHandler::setupAxises()
{
    for(auto axis : axises)
    {
        axis->setup();
    }
}

Point MovementsHandler::getAxisesLength()
{
    Point _axisesLength(axises.size());
    for(unsigned int i = 0; i < axises.size(); i++)
    {
        _axisesLength[i] = axises[i]->getLength();
    }
    return _axisesLength;
}

void MovementsHandler::moveTo(Point &to, bool mode)
{
    if(mode)
    {
        // точка задана относительно базы
        if(checkCurrentCoordinates(to))
        {

            //currentCoordinatesFromBase += offset*current_step;
        }
    }
    //double current_step = (step > 0) ? step : 0.01;
}

void MovementsHandler::move(const Point& offset)
{
    //double current_step = (step > 0) ? step : 0.01;
    Point newCoordinates = currentCoordinates + offset;
    if(checkCurrentCoordinates(newCoordinates))
    {
        currentCoordinatesFromBase = newCoordinates;
    }
}

bool MovementsHandler::checkCurrentCoordinates(Point &newCoordinates)
{
    bool isMovementCorrect = true;
    if(!dimensionsManager.isMovementCorrect(axisesLength, newCoordinates))
    {
        isMovementCorrect = false;
    }
    return isMovementCorrect;
}

Point MovementsHandler::getCurrentCoordinatesFromBase() const
{
    return currentCoordinatesFromBase;
}

Point MovementsHandler::getParkCoordinates() const
{
    return parkCoordinates;
}

void MovementsHandler::updateAllCoordinates(Point currentCoordinatesAbsolute)
{
    currentCoordinatesFromBase = currentCoordinatesAbsolute;

}

Point MovementsHandler::getCurrentCoordinates() const
{
    return currentCoordinates;
}

std::vector<std::shared_ptr<Axis> > MovementsHandler::getAxises() const
{
    return axises;
}

void MovementsHandler::setAxises(const std::vector<std::shared_ptr<Axis> > &value)
{
    axises = value;
}

