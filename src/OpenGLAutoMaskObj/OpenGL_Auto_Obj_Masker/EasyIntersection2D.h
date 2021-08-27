#ifndef EASYINTERSECTION_2D
#define EASYINTERSECTION_2D

#include <iostream>
#include <vector>

#include "EasyPoint2D.h"

class EasyIntersection2D
{
public:
    EasyIntersection2D()
    {
    }

    bool setPosition(
        const float &position_x,
        const float &position_y);

    bool setPosition(
        const EasyPoint2D &point_data);

    bool addPolygonIdx(
        const size_t &polygon_id,
        const size_t &line_idx);

    EasyPoint2D point;
    std::vector<std::pair<size_t, size_t>> polygon_idx_pair_vec;
};

#endif //EASYINTERSECTION_2D
