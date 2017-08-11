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

