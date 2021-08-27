#include "EasyIntersection2D.h"

bool EasyIntersection2D::setPosition(
    const float &position_x,
    const float &position_y)
{
    point.x = position_x;
    point.y = position_y;

    return true;
}

bool EasyIntersection2D::setPosition(
    const EasyPoint2D &point_data)
{
    return setPosition(point_data.x, point_data.y);
}

bool EasyIntersection2D::addPolygonIdx(
    const size_t &polygon_id,
    const size_t &line_idx)
{
    for(const std::pair<size_t, size_t> polygon_idx_pair :
        polygon_idx_pair_vec)
    {
        if(polygon_idx_pair.first == polygon_id &&
            polygon_idx_pair.second == line_idx)
        {
            return true;
        }
    }

    std::pair<size_t, size_t> new_polygon_idx_pair;
    new_polygon_idx_pair.first = polygon_id;
    new_polygon_idx_pair.second = line_idx;

    polygon_idx_pair_vec.emplace_back(new_polygon_idx_pair);

    return true;
}

