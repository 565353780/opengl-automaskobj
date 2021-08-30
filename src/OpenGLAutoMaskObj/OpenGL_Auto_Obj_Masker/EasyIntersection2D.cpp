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

bool EasyIntersection2D::haveThisPointIdx(
    const size_t &polygon_idx,
    const size_t &point_idx) const
{
    for(const std::pair<size_t, std::vector<size_t>> &polygon_point_idx_vec_pair :
        polygon_point_idx_vec_pair_vec)
    {
        if(polygon_point_idx_vec_pair.first != polygon_idx)
        {
            continue;
        }

        for(const size_t &polygon_point_idx : polygon_point_idx_vec_pair.second)
        {
            if(polygon_point_idx == point_idx)
            {
                return true;
            }
        }
    }

    return false;
}

bool EasyIntersection2D::addPolygonPointIdx(
    const size_t &polygon_idx,
    const size_t &point_idx)
{
    if(haveThisPointIdx(polygon_idx, point_idx))
    {
        return true;
    }

    std::pair<size_t, std::vector<size_t>> new_polygon_line_idx_pair;
    new_polygon_line_idx_pair.first = polygon_idx;
    new_polygon_line_idx_pair.second.emplace_back(point_idx);

    polygon_point_idx_vec_pair_vec.emplace_back(new_polygon_line_idx_pair);

    return true;
}

