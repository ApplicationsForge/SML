#include "pointsmanager.h"

PointsManager::PointsManager()
{
    // резервируем некоторое количество точек наперед, чтобы при добавлении не происходило перераспределение памяти вектора
    m_points.reserve(32);
}

PointsManager::PointsManager(const PointsManager &object) :
    m_points(object.m_points)
{

}

PointsManager::~PointsManager()
{

}

void PointsManager::addPoint(Point *p)
{
    std::shared_ptr<Point> ptr(p);
    m_points.push_back(ptr);
}

void PointsManager::deletePoint(Point *p)
{
    deletePoint(std::shared_ptr<Point>(p));
}

void PointsManager::deletePoint(const std::shared_ptr<Point>& p)
{
    m_points.erase( std::remove(m_points.begin(), m_points.end(), p), m_points.end() );
}

void PointsManager::deletePoint(size_t idx)
{
    std::shared_ptr<Point> point = operator [](idx);
    deletePoint(point);
}

void PointsManager::deletePoints(size_t beginIndex, size_t endIndex)
{
    size_t pointsSize = pointCount();
    if((beginIndex <=  pointsSize) && (endIndex <= pointsSize))
    {
        m_points.erase(m_points.begin() + beginIndex, m_points.begin() + endIndex);
    }
}

size_t PointsManager::pointCount() const
{
    return m_points.size();
}

void PointsManager::setCoordinatesCount(size_t num)
{
    for (auto& point : m_points)
    {
        point->setCoordinatesCount(num);
    }
}

std::shared_ptr<Point>& PointsManager::operator[](size_t idx)
{
    if (idx < pointCount())
    {
        return m_points[idx];
    }
    else
    {
        std::string errMsg = "Нет точки с номером " + std::to_string(idx);
        errMsg += " (Всего " + std::to_string(pointCount()) + " точек)";

        throw std::out_of_range(errMsg);
    }
}

Point3D PointsManager::getPoint3D(QString idx)
{
    Point3D point3d;
    try
    {
        bool isNumber = true;
        size_t pointNumberValue = idx.toUInt(&isNumber);
        if(isNumber)
        {
            std::shared_ptr<Point> pointOriginal = this->operator [](pointNumberValue-1);
            point3d = PointsManager::toPoint3D(pointOriginal);
        }
    }
    catch(...)
    {
        point3d = Point3D(-1, -1, -1);
        throw;
    }
    return point3d;
}

Point3D PointsManager::toPoint3D(std::shared_ptr<Point> origin)
{
    Point3D point3d;
    try
    {
        point3d.x = origin->get("X");
        point3d.y = origin->get("Y");
        point3d.z = origin->get("Z");
    }
    catch(...)
    {
        point3d = Point3D(-1, -1, -1);
    }
    return point3d;
}

QList<QStringList> PointsManager::points()
{
    QList<QStringList> points;
    for(auto point : m_points)
    {
        QStringList coordinates;
        unsigned int coordinatesCount = point.get()->size();
        for(unsigned int j = 0; j < coordinatesCount; j++)
        {
            QString coordinate;
            try
            {
                coordinate = QString::number(point.get()->operator [](j));
            }
            catch(std::out_of_range e)
            {
                QMessageBox(QMessageBox::Warning, "Ошибка", e.what()).exec();
                break;
            }
            coordinates.push_back(coordinate);
        }
        points.push_back(coordinates);

    }

    /*for(unsigned int i = 0; i < pointsCount; i++)
    {
        std::shared_ptr<Point> point = pointsManager->operator [](i);

        QStringList coordinates;
        unsigned int coordinatesCount = point.get()->size();
        for(unsigned int j = 0; j < coordinatesCount; j++)
        {
            QString coordinate;
            try
            {
                coordinate = QString::number(point.get()->operator [](j));
            }
            catch(std::out_of_range e)
            {
                QMessageBox(QMessageBox::Warning, "Ошибка", e.what()).exec();
                break;
            }
            coordinates.push_back(coordinate);
        }
        points.push_back(coordinates);
    }*/
    return points;
}

QStringList PointsManager::point(unsigned int number)
{
    QStringList coordinates;
    try
    {
        std::shared_ptr<Point> p = this->operator [](number);
        unsigned int coordinatesCount = p.get()->size();
        for(unsigned int j = 0; j < coordinatesCount; j++)
        {
            QString coordinate;
            coordinate = QString::number(p.get()->operator [](j));
            coordinates.push_back(coordinate);
        }
    }
    catch(std::out_of_range e)
    {
        QMessageBox(QMessageBox::Warning, "Ошибка", e.what()).exec();
    }
    return coordinates;
}

Point *PointsManager::makePoint(QStringList arguments)
{
    std::vector<double> pointsCoordinates;
    for(auto argument : arguments)
    {
        pointsCoordinates.push_back(argument.toDouble());
    }
    Point* p = new Point(pointsCoordinates);
    return p;
}

