#include "EasyMask2D.h"

bool EasyMask2D::getUnionPolygonVec(
    EasyPolygon2D &polygon_1,
    EasyPolygon2D &polygon_2,
    std::vector<EasyPolygon2D> &union_polygon_vec)
{
    union_polygon_vec.clear();

    if(polygon_1.point_list.size() == 0)
    {
        if(polygon_2.point_list.size() == 0)
        {
            return true;
        }

        union_polygon_vec.emplace_back(polygon_2);

        return true;
    }
    else if(polygon_2.point_list.size() == 0)
    {
        union_polygon_vec.emplace_back(polygon_1);

        return true;
    }

    std::vector<EasyIntersection2D> intersection_vec;
    getPolygonIntersection(polygon_1, polygon_2, intersection_vec);

    std::vector<EasyPolygon2D> inner_polygon_vec;

    std::vector<bool> polygon_1_line_connected_vec;
    std::vector<bool> polygon_2_line_connected_vec;
    std::vector<bool> intersection_connected_vec;
    polygon_1_line_connected_vec.resize(polygon_1.point_list.size(), false);
    polygon_2_line_connected_vec.resize(polygon_2.point_list.size(), false);

    int connected_polygon_line_num = 0;
    int total_polygon_line_num =
      polygon_1.line_list.size() +
      polygon_2.line_list.size();

    while(connected_polygon_line_num < total_polygon_line_num)
    {
        EasyPolygon2D current_union_polygon;

        float x_min = polygon_1.line_list[0].point_1.x;
        float x_min_2 = polygon_1.line_list[0].point_2.x;
        int current_start_polygon_id = polygon_1.id;
        int current_start_line_idx = 0;
        int current_start_intersection_idx = -1;
        for(size_t i = 1; i < polygon_1.line_list.size(); ++i)
        {
            if(polygon_1.line_list[i].point_1.x < x_min)
            {
                x_min = polygon_1.line_list[i].point_1.x;
                current_start_polygon_id = polygon_1.id;
                current_start_line_idx = i;
            }
            else if(polygon_1.line_list[i].point_1.x == x_min)
            {
                if(polygon_1.line_list[i].point_2.x < x_min_2)
                {
                    x_min_2 = polygon_1.line_list[i].point_2.x;
                    current_start_polygon_id = polygon_1.id;
                    current_start_line_idx = i;
                }
            }
        }
        for(size_t i = 0; i < polygon_2.line_list.size(); ++i)
        {
            if(polygon_2.line_list[i].point_1.x < x_min)
            {
                x_min = polygon_2.line_list[i].point_1.x;
                current_start_polygon_id = polygon_2.id;
                current_start_line_idx = i;
            }
            else if(polygon_2.line_list[i].point_1.x == x_min)
            {
                if(polygon_2.line_list[i].point_2.x < x_min_2)
                {
                    x_min_2 = polygon_2.line_list[i].point_2.x;
                    current_start_polygon_id = polygon_2.id;
                    current_start_line_idx = i;
                }
            }
        }

        // std::cout << "current polygon_1 :" << std::endl <<
          // "points :" << std::endl;
        // for(int i = 0; i < polygon_1.point_list.size(); ++i)
        // {
            // std::cout << "\t" << i << " : [" <<
              // polygon_1.point_list[i].x << "," <<
              // polygon_1.point_list[i].y << "]" << std::endl;
        // }
        // std::cout << "current polygon_2 :" << std::endl <<
          // "points :" << std::endl;
        // for(int i = 0; i < polygon_2.point_list.size(); ++i)
        // {
            // std::cout << "\t" << i << " : [" <<
              // polygon_2.point_list[i].x << "," <<
              // polygon_2.point_list[i].y << "]" << std::endl;
        // }
        // std::cout << "current intersection:" << std::endl;
        // for(int i = 0; i < intersection_vec.size(); ++i)
        // {
            // std::cout << "points :" << std::endl;
            // std::cout << "\t" << i << " : [" <<
              // intersection_vec[i].point.x << "," <<
              // intersection_vec[i].point.y << "]" << std::endl;
            // std::cout << "idx :" << std::endl;
            // for(int j = 0;
                // j < intersection_vec[i].polygon_idx_pair_vec.size();
                // ++j)
            // {
                // std::cout << "\t" << j << " : [" <<
                  // intersection_vec[i].polygon_idx_pair_vec[j].first <<
                  // "," <<
                  // intersection_vec[i].polygon_idx_pair_vec[j].second <<
                  // "]" << std::endl;
            // }
        // }

        int start_line_idx = current_start_line_idx;

        addNewPolygonPoint(
            polygon_1,
            polygon_2,
            current_start_polygon_id,
            current_start_line_idx,
            current_union_polygon,
            polygon_1_line_connected_vec,
            polygon_2_line_connected_vec);

        startLineIdxAdd1(
            polygon_1,
            polygon_2,
            current_start_polygon_id,
            current_start_line_idx);

        getUnionPolygonPoints(
            polygon_1,
            polygon_2,
            intersection_vec,
            current_start_polygon_id,
            start_line_idx,
            current_start_polygon_id,
            current_start_line_idx,
            current_start_intersection_idx,
            current_union_polygon,
            polygon_1_line_connected_vec,
            polygon_2_line_connected_vec);

        current_union_polygon.update();

        union_polygon_vec.emplace_back(current_union_polygon);

        // std::cout << "current union_polygon:" << std::endl <<
          // "points :" << std::endl;
        // for(int i = 0; i < current_union_polygon.point_list.size(); ++i)
        // {
            // std::cout << "\t" << i << " : [" <<
              // current_union_polygon.point_list[i].x << "," <<
              // current_union_polygon.point_list[i].y << "]" << std::endl;
        // }

        updateAllPolygonLineConnectedState(
            polygon_1,
            current_union_polygon,
            polygon_1_line_connected_vec);
        updateAllPolygonLineConnectedState(
            polygon_2,
            current_union_polygon,
            polygon_2_line_connected_vec);

        connected_polygon_line_num =
          count(
              polygon_1_line_connected_vec.begin(),
              polygon_1_line_connected_vec.end(),
              true) +
          count(
              polygon_2_line_connected_vec.begin(),
              polygon_2_line_connected_vec.end(),
              true);
    }

    return true;
}

float EasyMask2D::dot(
    const float &x_1,
    const float &y_1,
    const float &x_2,
    const float &y_2)
{
    return x_1 * x_2 + y_1 * y_2;
}

float EasyMask2D::dot(
    const EasyLine2D &line_1,
    const EasyLine2D &line_2)
{
    return dot(line_1.x_diff, line_1.y_diff, line_2.x_diff, line_2.y_diff);
}

float EasyMask2D::cross(
    const float &x_1,
    const float &y_1,
    const float &x_2,
    const float &y_2)
{
    return x_1 * y_2 - x_2 * y_1;
}

float EasyMask2D::cross(
    const EasyLine2D &line_1,
    const EasyLine2D &line_2)
{
    return cross(line_1.x_diff, line_1.y_diff, line_2.x_diff, line_2.y_diff);
}

float EasyMask2D::pointDist2(
    const EasyPoint2D &point_1,
    const EasyPoint2D &point_2)
{
    float dist_2 =
      pow(point_1.x - point_2.x, 2) +
      pow(point_1.y - point_2.y, 2);

    return dist_2;
}

float EasyMask2D::pointDist(
    const EasyPoint2D &point_1,
    const EasyPoint2D &point_2)
{
    return sqrt(pointDist2(point_1, point_2));
}

float EasyMask2D::lineLength2(
    const EasyLine2D &line)
{
    float length_2 = pow(line.x_diff, 2) + pow(line.y_diff, 2);

    return length_2;
}

float EasyMask2D::lineLength(
    const EasyLine2D &line)
{
    return sqrt(lineLength2(line));
}

float EasyMask2D::angle(
    const EasyLine2D &line_1,
    const EasyLine2D &line_2)
{
    int sign = 1;

    float cross_value = cross(line_1, line_2);

    if(cross_value == 0)
    {
        return 0;
    }

    if(cross_value < 0)
    {
        sign = -1;
    }

    float dot_value = dot(line_1, line_2);

    float line_1_length = lineLength(line_1);
    float line_2_length = lineLength(line_2);

    float cos_value = dot_value / (line_1_length * line_2_length);

    float angle_value = sign * acos(cos_value);

    return angle_value;
}

float EasyMask2D::clockWiseAngle(
    const EasyLine2D &line_1,
    const EasyLine2D &line_2)
{
    if(cross(line_1, line_2) < 0)
    {
        return angle(line_1, line_2) + 6.28;
    }

    return angle(line_1, line_2);
}

bool EasyMask2D::isPointInRect(
    const EasyPoint2D &point,
    const EasyRect2D &rect)
{
    if(point.x < rect.x_min)
    {
        return false;
    }

    if(point.x > rect.x_max)
    {
        return false;
    }

    if(point.y < rect.y_min)
    {
        return false;
    }

    if(point.y > rect.y_max)
    {
        return false;
    }

    return true;
}

bool EasyMask2D::isPointInLineRect(
    const EasyPoint2D &point,
    const EasyLine2D &line)
{
    if(point.x < line.rect.x_min)
    {
        return false;
    }

    if(point.x > line.rect.x_max)
    {
        return false;
    }

    if(point.y < line.rect.y_min)
    {
        return false;
    }

    if(point.y > line.rect.y_max)
    {
        return false;
    }

    return true;
}

bool EasyMask2D::isRectCross(
    const EasyLine2D &line_1,
    const EasyLine2D &line_2)
{
    if(line_1.rect.x_max < line_2.rect.x_min)
    {
        return false;
    }

    if(line_1.rect.x_min > line_2.rect.x_max)
    {
        return false;
    }

    if(line_1.rect.y_max < line_2.rect.y_min)
    {
        return false;
    }

    if(line_1.rect.y_min > line_2.rect.y_max)
    {
        return false;
    }

    return true;
}

bool EasyMask2D::isRectCross(
    const EasyRect2D &rect_1,
    const EasyRect2D &rect_2)
{
    if(rect_1.x_max < rect_2.x_min)
    {
        return false;
    }

    if(rect_1.x_min > rect_2.x_max)
    {
        return false;
    }

    if(rect_1.y_max < rect_2.y_min)
    {
        return false;
    }

    if(rect_1.y_min > rect_2.y_max)
    {
        return false;
    }

    return true;
}

bool EasyMask2D::isLineCross(
    const EasyLine2D &line_1,
    const EasyLine2D &line_2)
{
    if(!isRectCross(line_1, line_2))
    {
        return false;
    }

    EasyLine2D line_22_to_11;
    EasyLine2D line_22_to_12;

    line_22_to_11.setPosition(
        line_2.point_2,
        line_1.point_1);
    line_22_to_12.setPosition(
        line_2.point_2,
        line_1.point_2);

    float line_1_point_1_cross_line_2 =
      cross(line_22_to_11, line_2);
    float line_1_point_2_corss_line_2 =
      cross(line_22_to_12, line_2);

    if(line_1_point_1_cross_line_2 == 0)
    {
        if(line_1_point_2_corss_line_2 == 0)
        {
            return true;
        }

        return isPointInLineRect(line_1.point_1, line_2);
    }

    if(line_1_point_2_corss_line_2 == 0)
    {
        return isPointInLineRect(line_1.point_2, line_2);
    }

    if(line_1_point_1_cross_line_2 * line_1_point_2_corss_line_2 < 0)
    {
        return true;
    }

    return false;
}

bool EasyMask2D::isLineParallel(
    const EasyLine2D &line_1,
    const EasyLine2D &line_2)
{
    if(cross(line_1, line_2) == 0)
    {
        return true;
    }

    return false;
}

bool EasyMask2D::isPointOnOpenBoundedLine(
    const EasyPoint2D &point,
    const EasyLine2D &line)
{
    if(line.point_1.x != line.point_2.x)
    {
        if(point.x > line.rect.x_min && point.x < line.rect.x_max)
        {
            return true;
        }
    }
    else if(line.point_1.y != line.point_2.y)
    {
        if(point.y > line.rect.y_min && point.y < line.rect.y_max)
        {
            return true;
        }
    }

    return false;
}

bool EasyMask2D::isPointInPolygon(
    const EasyPoint2D &point,
    const EasyPolygon2D &polygon)
{
    float angle_value_sum = 0;

    for(size_t i = 0; i < polygon.point_list.size(); ++i)
    {
        int next_point_idx = i + 1;
        if(i == polygon.point_list.size() - 1)
        {
            next_point_idx = 0;
        }

        EasyLine2D line_1;
        EasyLine2D line_2;
        line_1.setPosition(
            point,
            polygon.point_list[i]);
        line_2.setPosition(
            point,
            polygon.point_list[next_point_idx]);

        angle_value_sum += angle(line_1, line_2);
    }

    float angle_value_sum_to_0 = fabs(angle_value_sum);
    float angle_value_sum_to_pi = fabs(angle_value_sum_to_0 - 3.14);
    float angle_value_sum_to_2pi = fabs(angle_value_sum - 6.28);

    if(angle_value_sum_to_0 < angle_value_sum_to_pi)
    {
        return false;
    }

    if(angle_value_sum_to_2pi < angle_value_sum_to_pi)
    {
        return true;
    }

    // here is the case point on the bound of polygon
    return true;
}

bool EasyMask2D::isPolygonCross(
    EasyPolygon2D &polygon_1,
    EasyPolygon2D &polygon_2)
{
    EasyRect2D pylygon_1_rect;
    EasyRect2D polygon_2_rect;

    polygon_1.getPolygonRect(pylygon_1_rect);
    polygon_2.getPolygonRect(polygon_2_rect);

    if(!isRectCross(pylygon_1_rect, polygon_2_rect))
    {
        return false;
    }

    for(const EasyPoint2D &point : polygon_1.point_list)
    {
        if(isPointInPolygon(point, polygon_2))
        {
            return true;
        }
    }

    return false;
}

bool EasyMask2D::getLineCrossPoint(
    const EasyLine2D &line_1,
    const EasyLine2D &line_2,
    EasyPoint2D &line_cross_point)
{
    float line_cross = cross(line_1, line_2);

    if(line_cross == 0)
    {
        std::cout << "EasyPolygon2D::getLineCrossPoint : lines are parallel!" << std::endl;
        return false;
    }

    float line_1_weight =
      (line_2.point_2.y - line_2.point_1.y) * line_2.point_1.x +
      (line_2.point_1.x - line_2.point_2.x) * line_2.point_1.y;

    float line_2_weight =
      (line_1.point_2.y - line_1.point_1.y) * line_1.point_1.x +
      (line_1.point_1.x - line_1.point_2.x) * line_1.point_1.y;

    float line_cross_point_x_weight =
      line_1_weight * (line_1.point_2.x - line_1.point_1.x) -
      line_2_weight * (line_2.point_2.x - line_2.point_1.x);

    float line_cross_point_y_weight =
      line_1_weight * (line_1.point_2.y - line_1.point_1.y) -
      line_2_weight * (line_2.point_2.y - line_2.point_1.y);

    line_cross_point.x = line_cross_point_x_weight / line_cross;
    line_cross_point.y = line_cross_point_y_weight / line_cross;

    return true;
}

bool EasyMask2D::getBoundedLineCrossPointVec(
    const EasyLine2D &line_1,
    const EasyLine2D &line_2,
    std::vector<EasyPoint2D> &line_cross_point_vec)
{
    line_cross_point_vec.clear();

    if(!isLineCross(line_1, line_2))
    {
        return true;
    }

    if(isLineParallel(line_1, line_2))
    {
        if(isSamePoint(line_1.point_1, line_2.point_1))
        {
            line_cross_point_vec.emplace_back(line_1.point_1);

            if(isSamePoint(line_1.point_2, line_2.point_2))
            {
                line_cross_point_vec.emplace_back(line_1.point_2);
            }
        }
        else if(isSamePoint(line_1.point_2, line_2.point_1))
        {
            line_cross_point_vec.emplace_back(line_1.point_2);

            if(isSamePoint(line_1.point_1, line_2.point_2))
            {
                line_cross_point_vec.emplace_back(line_1.point_1);
            }
        }

        if(isPointOnOpenBoundedLine(line_1.point_1, line_2))
        {
            line_cross_point_vec.emplace_back(line_1.point_1);
        }
        if(isPointOnOpenBoundedLine(line_1.point_2, line_2))
        {
            line_cross_point_vec.emplace_back(line_1.point_2);
        }
        if(isPointOnOpenBoundedLine(line_2.point_1, line_1))
        {
            line_cross_point_vec.emplace_back(line_2.point_1);
        }
        if(isPointOnOpenBoundedLine(line_2.point_2, line_1))
        {
            line_cross_point_vec.emplace_back(line_2.point_2);
        }

        return true;
    }

    EasyPoint2D line_cross_point;

    getLineCrossPoint(line_1, line_2, line_cross_point);

    line_cross_point_vec.emplace_back(line_cross_point);

    return true;
}

bool EasyMask2D::isSamePoint(
    const EasyPoint2D &point_1,
    const EasyPoint2D &point_2)
{
    if(point_1.x == point_2.x && point_1.y == point_2.y)
    {
        return true;
    }

    return false;
}

bool EasyMask2D::getPolygonIntersection(
    EasyPolygon2D &polygon_1,
    EasyPolygon2D &polygon_2,
    std::vector<EasyIntersection2D> &intersection_vec)
{
    intersection_vec.clear();

    if(!isPolygonCross(polygon_1, polygon_2))
    {
        return true;
    }


    for(size_t i = 0; i < polygon_1.line_list.size(); ++i)
    {
        for(size_t j = 0; j < polygon_2.line_list.size(); ++j)
        {
            std::vector<EasyPoint2D> line_cross_point_vec;
            getBoundedLineCrossPointVec(
                polygon_1.line_list[i],
                polygon_2.line_list[j],
                line_cross_point_vec);

            if(line_cross_point_vec.size() > 0)
            {
                for(const EasyPoint2D &line_cross_point : line_cross_point_vec)
                {
                    bool cross_point_exist = false;
                    for(EasyIntersection2D &exist_intersection : intersection_vec)
                    {
                        if(isSamePoint(exist_intersection.point, line_cross_point))
                        {
                            exist_intersection.addPolygonIdx(polygon_1.id, i);
                            exist_intersection.addPolygonIdx(polygon_2.id, j);
                            cross_point_exist = true;
                            break;
                        }
                    }

                    if(!cross_point_exist)
                    {
                        EasyIntersection2D intersection;
                        intersection.setPosition(line_cross_point);
                        intersection.addPolygonIdx(polygon_1.id, i);
                        intersection.addPolygonIdx(polygon_2.id, j);

                        intersection_vec.emplace_back(intersection);
                    }
                }
            }
        }
    }

    return true;
}

bool EasyMask2D::haveThisPolygonIdx(
    const EasyIntersection2D &intersection_2d,
    const size_t &polygon_id,
    const size_t &line_idx)
{
    for(const std::pair<size_t, size_t> &polygon_idx_pair : intersection_2d.polygon_idx_pair_vec)
    {
        if(polygon_idx_pair.first == polygon_id)
        {
            if(polygon_idx_pair.second == line_idx)
            {
                return true;
            }
        }
    }

    return false;
}

bool EasyMask2D::getIntersectionIdxVecOnPolygonLine(
    const std::vector<EasyIntersection2D> &intersection_vec,
    const size_t &polygon_id,
    const size_t &line_idx,
    std::vector<size_t> &intersection_idx_vec)
{
    intersection_idx_vec.clear();

    for(size_t i = 0; i < intersection_vec.size(); ++i)
    {
        if(haveThisPolygonIdx(intersection_vec[i], polygon_id, line_idx))
        {
            intersection_idx_vec.emplace_back(i);
        }
    }

    return true;
}

bool EasyMask2D::updateAllPolygonLineConnectedState(
    const EasyPolygon2D &polygon,
    const EasyPolygon2D &union_polygon,
    std::vector<bool> &polygon_line_connected_vec)
{
    std::vector<bool> polygon_point_connected_vec;
    std::vector<bool> polygon_point_computed_vec;
    polygon_point_connected_vec.resize(polygon.point_list.size(), false);
    polygon_point_computed_vec.resize(polygon.point_list.size(), false);

    for(size_t i = 0; i < polygon.point_list.size(); ++i)
    {
        if(!polygon_line_connected_vec[i])
        {
            if(!polygon_point_computed_vec[i])
            {
                if(isPointInPolygon(
                      polygon.point_list[i],
                      union_polygon))
                {
                    polygon_point_connected_vec[i] = true;
                }
                polygon_point_computed_vec[i] = true;
            }

            size_t next_point_idx = i + 1;
            if(i == polygon.point_list.size() - 1)
            {
                next_point_idx = 0;
            }

            if(!polygon_point_computed_vec[next_point_idx])
            {
                if(isPointInPolygon(
                      polygon.point_list[next_point_idx],
                      union_polygon))
                {
                    polygon_point_connected_vec[next_point_idx] = true;
                }
                polygon_point_computed_vec[next_point_idx] = true;
            }
        }
    }

    for(size_t i = 0; i < polygon_line_connected_vec.size(); ++i)
    {
        if(!polygon_line_connected_vec[i])
        {
            size_t next_point_idx = i + 1;
            if(i == polygon_line_connected_vec.size())
            {
                i = 0;
            }
            if(polygon_point_connected_vec[i] &&
                polygon_point_connected_vec[next_point_idx])
            {
                polygon_line_connected_vec[i] = true;
            }
        }
    }
    return true;
}

bool EasyMask2D::addNewPolygonPoint(
    const EasyPolygon2D &polygon_1,
    const EasyPolygon2D &polygon_2,
    const int &polygon_id,
    const int &start_line_idx,
    EasyPolygon2D &union_polygon,
    std::vector<bool> &polygon_1_line_connected_vec,
    std::vector<bool> &polygon_2_line_connected_vec)
{
    if(polygon_id == polygon_1.id)
    {
        union_polygon.addPoint(polygon_1.line_list[start_line_idx].point_1);
        polygon_1_line_connected_vec[start_line_idx] = true;

        return true;
    }

    if(polygon_id == polygon_2.id)
    {
        union_polygon.addPoint(polygon_2.line_list[start_line_idx].point_1);
        polygon_2_line_connected_vec[start_line_idx] = true;

        return true;
    }

    std::cout << "EasyMask2D::addNewPolygonPoint : polygon not found!" << std::endl;

    return false;
}

bool EasyMask2D::startLineIdxAdd1(
    const EasyPolygon2D &polygon_1,
    const EasyPolygon2D &polygon_2,
    const int &polygon_id,
    int &start_line_idx)
{
    if(polygon_id == polygon_1.id)
    {
        if((size_t)start_line_idx == polygon_1.line_list.size() - 1)
        {
            start_line_idx = 0;

            return true;
        }

        ++start_line_idx;

        return true;
    }

    if(polygon_id == polygon_2.id)
    {
        if((size_t)start_line_idx == polygon_2.line_list.size() - 1)
        {
            start_line_idx = 0;

            return true;
        }

        ++start_line_idx;

        return true;
    }

    std::cout << "EasyMask2D::StartLineIdxAdd1 : polygon not found!" << std::endl;

    return false;
}

bool EasyMask2D::getNearestIntersectionIdx(
    const EasyPolygon2D &polygon_1,
    const EasyPolygon2D &polygon_2,
    const std::vector<EasyIntersection2D> &intersection_vec,
    const int &polygon_id,
    const int &start_line_idx,
    const std::vector<size_t> &intersection_idx_vec,
    int &nearest_intersection_idx)
{
    nearest_intersection_idx = -1;

    if(intersection_idx_vec.size() == 0)
    {
        std::cout << "EasyMask2D::getNearestIntersectionIdx : no intersection!" << std::endl;
        return false;
    }

    float min_dist_to_intersection = -1;

    if(polygon_id == polygon_1.id)
    {
        for(size_t i = 0; i < intersection_idx_vec.size(); ++i)
        {
            float current_min_dist =
              pointDist2(
                  intersection_vec[intersection_idx_vec[i]].point,
                  polygon_1.line_list[start_line_idx].point_1);

            if(current_min_dist == 0)
            {
                continue;
            }

            if(min_dist_to_intersection == -1 ||
                current_min_dist < min_dist_to_intersection)
            {
                min_dist_to_intersection = current_min_dist;
                nearest_intersection_idx = intersection_idx_vec[i];
            }
        }

        return true;
    }

    if(polygon_id == polygon_2.id)
    {
        for(size_t i = 0; i < intersection_idx_vec.size(); ++i)
        {
            float current_min_dist =
              pointDist2(
                  intersection_vec[intersection_idx_vec[i]].point,
                  polygon_2.line_list[start_line_idx].point_1);

            if(current_min_dist == 0)
            {
                continue;
            }

            if(min_dist_to_intersection == -1 ||
                current_min_dist < min_dist_to_intersection)
            {
                min_dist_to_intersection = current_min_dist;
                nearest_intersection_idx = intersection_idx_vec[i];
            }
        }

        return true;
    }

    std::cout << "EasyMask2D::getNearestIntersectionIdx : polygon not found!" << std::endl;
    return false;
}

bool EasyMask2D::getMinimalAnglePolygonPointIdx(
    const EasyPolygon2D &polygon_1,
    const EasyPolygon2D &polygon_2,
    const std::vector<EasyIntersection2D> &intersection_vec,
    const int &polygon_id,
    const int &start_line_idx,
    const int &start_intersection_idx,
    std::pair<int, int> &minimal_angle_polygon_point_idx)
{
    minimal_angle_polygon_point_idx.first = -1;
    minimal_angle_polygon_point_idx.second = -1;

    float minimal_angle = 6.28;
    EasyLine2D line_intersection_to_polygon_line_point_1;
    EasyLine2D line_intersection_to_polygon_line_point_2;

    if(polygon_id == polygon_1.id)
    {
        line_intersection_to_polygon_line_point_1.setPosition(
            intersection_vec[start_intersection_idx].point,
            polygon_1.line_list[start_line_idx].point_1);
    }
    else if(polygon_id == polygon_2.id)
    {
        line_intersection_to_polygon_line_point_1.setPosition(
            intersection_vec[start_intersection_idx].point,
            polygon_2.line_list[start_line_idx].point_1);
    }
    else
    {
        std::cout << "EasyMask2D::getMinimalAnglePolygonPointIdx : polygon not found!" << std::endl;
        return false;
    }

    for(const std::pair<size_t, size_t> &polygon_idx_pair :
        intersection_vec[start_intersection_idx].polygon_idx_pair_vec)
    {
        if(polygon_idx_pair.first == polygon_1.id)
        {
            line_intersection_to_polygon_line_point_2.setPosition(
                intersection_vec[start_intersection_idx].point,
                polygon_1.line_list[polygon_idx_pair.second].point_2);
        }
        else if(polygon_idx_pair.first == polygon_2.id)
        {
            line_intersection_to_polygon_line_point_2.setPosition(
                intersection_vec[start_intersection_idx].point,
                polygon_2.line_list[polygon_idx_pair.second].point_2);
        }
        else
        {
            continue;
        }

        float current_angle = clockWiseAngle(
            line_intersection_to_polygon_line_point_1,
            line_intersection_to_polygon_line_point_2);

        if(current_angle == 0)
        {
            continue;
        }

        if(current_angle < minimal_angle)
        {
            minimal_angle = current_angle;
            minimal_angle_polygon_point_idx.first = polygon_idx_pair.first;
            minimal_angle_polygon_point_idx.second = polygon_idx_pair.second;
        }
    }

    return true;
}

bool EasyMask2D::getUnionPolygonPoints(
    const EasyPolygon2D &polygon_1,
    const EasyPolygon2D &polygon_2,
    const std::vector<EasyIntersection2D> &intersection_vec,
    const int &start_polygon_id,
    const int &start_line_idx,
    const int &current_start_polygon_id,
    const int &current_start_line_idx,
    const int &current_start_intersection_idx,
    EasyPolygon2D &union_polygon,
    std::vector<bool> &polygon_1_line_connected_vec,
    std::vector<bool> &polygon_2_line_connected_vec)
{
    int new_start_polygon_id = current_start_polygon_id;
    int new_start_line_idx = current_start_line_idx;
    int new_start_intersection_idx = current_start_intersection_idx;

    if(new_start_polygon_id == start_polygon_id &&
        new_start_line_idx == start_line_idx)
    {
        return true;
    }

    if(new_start_polygon_id == -1 ||
        new_start_line_idx == -1)
    {
        std::cout << "force exit!" << std::endl;
        exit(0);
    }

    if(new_start_intersection_idx == -1)
    {
        std::vector<size_t> intersection_idx_vec;

        addNewPolygonPoint(
            polygon_1,
            polygon_2,
            new_start_polygon_id,
            new_start_line_idx,
            union_polygon,
            polygon_1_line_connected_vec,
            polygon_2_line_connected_vec);

        getIntersectionIdxVecOnPolygonLine(
            intersection_vec,
            new_start_polygon_id,
            new_start_line_idx,
            intersection_idx_vec);

        if(intersection_idx_vec.size() == 0)
        {
            startLineIdxAdd1(
                polygon_1,
                polygon_2,
                new_start_polygon_id,
                new_start_line_idx);

            return getUnionPolygonPoints(
                polygon_1,
                polygon_2,
                intersection_vec,
                start_polygon_id,
                start_line_idx,
                new_start_polygon_id,
                new_start_line_idx,
                new_start_intersection_idx,
                union_polygon,
                polygon_1_line_connected_vec,
                polygon_2_line_connected_vec);
        }

        getNearestIntersectionIdx(
            polygon_1,
            polygon_2,
            intersection_vec,
            new_start_polygon_id,
            new_start_line_idx,
            intersection_idx_vec,
            new_start_intersection_idx);

        if(haveThisPolygonIdx(
              intersection_vec[new_start_intersection_idx],
              start_polygon_id,
              start_line_idx))
        {
            return true;
        }

        union_polygon.addPoint(
            intersection_vec[new_start_intersection_idx].point);

        return getUnionPolygonPoints(
            polygon_1,
            polygon_2,
            intersection_vec,
            start_polygon_id,
            start_line_idx,
            new_start_polygon_id,
            new_start_line_idx,
            new_start_intersection_idx,
            union_polygon,
            polygon_1_line_connected_vec,
            polygon_2_line_connected_vec);
    }

    std::pair<int, int> minimal_angle_polygon_point_idx_pair;
    getMinimalAnglePolygonPointIdx(
        polygon_1,
        polygon_2,
        intersection_vec,
        new_start_polygon_id,
        new_start_line_idx,
        new_start_intersection_idx,
        minimal_angle_polygon_point_idx_pair);

    new_start_polygon_id = minimal_angle_polygon_point_idx_pair.first;
    new_start_line_idx = minimal_angle_polygon_point_idx_pair.second;
    new_start_intersection_idx = -1;

    if(new_start_polygon_id == start_polygon_id &&
        new_start_line_idx == start_line_idx)
    {
        return true;
    }

    startLineIdxAdd1(
        polygon_1,
        polygon_2,
        new_start_polygon_id,
        new_start_line_idx);

    return getUnionPolygonPoints(
        polygon_1,
        polygon_2,
        intersection_vec,
        start_polygon_id,
        start_line_idx,
        new_start_polygon_id,
        new_start_line_idx,
        new_start_intersection_idx,
        union_polygon,
        polygon_1_line_connected_vec,
        polygon_2_line_connected_vec);

    return true;
}

