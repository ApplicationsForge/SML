#ifndef MOVEMENTSHANDLER_H
#define MOVEMENTSHANDLER_H

#include "models/structs.h"
#include "models/machinetool/movementshandler/axis/axis.h"
#include "models/machinetool/movementshandler/dimensionsmanager/dimensionsmanager.h"

class MovementsHandler
{
protected:
    /// Оси станка
    std::vector< std::shared_ptr<Axis> > axises;

    Point currentCoordinates;
    Point currentCoordinatesFromBase;
    Point parkCoordinates;
    Point axisesLength;
    // шаг движения
    double step;
    double velocity;
    DimensionsManager* dimensionsManager;
public:
    MovementsHandler(SettingsManager* sm);
    ~MovementsHandler();
    void addAxises(const unsigned int &count, SettingsManager *sm);
    std::vector<std::shared_ptr<Axis> > getAxises() const;
    void setAxises(const std::vector<std::shared_ptr<Axis> > &value);
    void moveTo(Point &to, bool mode = 0);
    void move(const Point &offset);
    bool checkCurrentCoordinates(Point &newCoordinates);
    Point getAxisesLength();
    Point getCurrentCoordinates() const;
    Point getCurrentCoordinatesFromBase() const;
    Point getParkCoordinates() const;

    void updateAllCoordinates(Point currentCoordinatesAbsolute);
};

#endif // MOVEMENTSHANDLER_H
