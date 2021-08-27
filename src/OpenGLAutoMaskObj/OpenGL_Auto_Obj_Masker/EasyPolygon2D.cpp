#include "EasyPolygon2D.h"

bool EasyPolygon2D::reset()
{
    point_list.clear();
    line_list.clear();
    id = -1;

    return true;
}

bool EasyPolygon2D::setID(
    const int &id_data)
{
    id = id_data;

    return true;
}

bool EasyPolygon2D::addPoint(
    const EasyPoint2D &point_2d)
{
    point_list.emplace_back(point_2d);

    return true;
}

bool EasyPolygon2D::insertPoint(
    const EasyPoint2D &point_2d,
    const size_t &insert_idx)
{
    if(insert_idx > point_list.size() - 1)
    {
        std::cout << "insert point to polygon failed!" << std::endl;
        return false;
    }

    point_list.insert(point_list.begin() + insert_idx, point_2d);

    return true;
}

bool EasyPolygon2D::removePoint(
    const size_t &remove_idx)
{
    if(remove_idx > point_list.size() - 1)
    {
        std::cout << "remove point from polygon failed!" << std::endl;
        return false;
    }

    point_list.erase(point_list.begin() + remove_idx);

    return true;
}

bool EasyPolygon2D::update()
{
    setAntiClockWise();
    updateLineVec();

    return true;
}

bool EasyPolygon2D::updateLineVec()
{
    line_list.resize(point_list.size());

    if(point_list.size() == 0)
    {
        return true;
    }

    for(size_t i = 0; i < point_list.size(); ++i)
    {
        int next_point_idx = i + 1;
        if(i == point_list.size() - 1)
        {
            next_point_idx = 0;
        }

        EasyLine2D new_line;
        line_list[i].setPosition(
            point_list[i],
            point_list[next_point_idx]);
    }

    return true;
}

bool EasyPolygon2D::isClockWise()
{
    if(getPolygonArea() < 0)
    {
        return true;
    }

    return false;
}

bool EasyPolygon2D::setClockWise()
{
    if(!isClockWise())
    {
        std::reverse(point_list.begin(), point_list.end());
    }

    return true;
}

bool EasyPolygon2D::setAntiClockWise()
{
    if(isClockWise())
    {
        std::reverse(point_list.begin(), point_list.end());
    }

    return true;
}

float EasyPolygon2D::getPolygonArea()
{
    if(point_list.size() < 3)
    {
        return 0;
    }

    float area = 0;
    
    for(size_t i = 0; i < point_list.size(); ++i)
    {
        int next_point_idx = i + 1;
        if(i == point_list.size() - 1)
        {
            next_point_idx = 0;
        }

        area -= 0.5 *
          (point_list[next_point_idx].y + point_list[i].y) *
          (point_list[next_point_idx].x - point_list[i].x);
    }

    return area;
}

bool EasyPolygon2D::getPolygonRect(
    EasyRect2D &rect_2d)
{
    if(point_list.size() == 0)
    {
        std::cout << "EasyPolygon2D::getPolygonRect : no point found!" << std::endl;
        return false;
    }

    rect_2d.x_min = point_list[0].x;
    rect_2d.x_max = rect_2d.x_min;
    rect_2d.y_min = point_list[0].y;
    rect_2d.y_max = rect_2d.y_min;

    for(const EasyPoint2D &point : point_list)
    {
        rect_2d.x_min = fmin(rect_2d.x_min, point.x);
        rect_2d.x_max = fmax(rect_2d.x_max, point.x);
        rect_2d.y_min = fmin(rect_2d.y_min, point.y);
        rect_2d.y_max = fmax(rect_2d.y_max, point.y);
    }

    return true;
}

