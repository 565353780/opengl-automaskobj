#include "EasyMask2D.h"

bool EasyMask2D::getUnionPolygonVec(
    std::vector<EasyPolygon2D> &polygon_vec,
    std::vector<EasyPolygon2D> &union_polygon_vec)
{
    union_polygon_vec.clear();

    std::vector<EasyPolygon2D> valid_polygon_vec;

    for(const EasyPolygon2D &polygon : polygon_vec)
    {
        if(polygon.point_list.size() > 0)
        {
            valid_polygon_vec.emplace_back(polygon);
        }
    }

    if(valid_polygon_vec.size() == 0)
    {
        return true;
    }

    if(valid_polygon_vec.size() == 1)
    {
        union_polygon_vec.emplace_back(valid_polygon_vec[0]);

        return true;
    }

    std::vector<EasyIntersection2D> intersection_vec;
    getPolygonIntersection(valid_polygon_vec, intersection_vec);

    std::vector<EasyPolygon2D> split_polygon_vec;
    std::vector<EasyIntersection2D> split_intersection_vec;

    splitPolygonsByIntersection(
        intersection_vec,
        valid_polygon_vec,
        split_intersection_vec,
        split_polygon_vec);

    size_t connected_polygon_point_num = 0;
    size_t total_polygon_point_num = 0;

    std::vector<std::vector<bool>> polygon_point_connected_vec_vec;
    polygon_point_connected_vec_vec.resize(split_polygon_vec.size());
    for(size_t i = 0; i < split_polygon_vec.size(); ++i)
    {
        polygon_point_connected_vec_vec[i].resize(split_polygon_vec[i].point_list.size(), false);
        total_polygon_point_num += split_polygon_vec[i].point_list.size();
    }

    while(connected_polygon_point_num < total_polygon_point_num)
    {
        EasyPolygon2D current_union_polygon;

        float x_min;
        float y_max;
        int current_start_polygon_idx = -1;
        int current_start_point_idx = -1;

        for(size_t i = 0; i < split_polygon_vec.size(); ++i)
        {
            EasyPolygon2D &split_polygon = split_polygon_vec[i];

            for(size_t j = 0; j < split_polygon.point_list.size(); ++j)
            {
                EasyPoint2D &polygon_point = split_polygon.point_list[j];

                if(!polygon_point_connected_vec_vec[i][j])
                {
                    if(current_start_polygon_idx == -1)
                    {
                        x_min = polygon_point.x;
                        y_max = polygon_point.y;
                        current_start_polygon_idx = i;
                        current_start_point_idx = j;
                    }
                    else if(polygon_point.x < x_min)
                    {
                        x_min = polygon_point.x;
                        current_start_polygon_idx = i;
                        current_start_point_idx = j;
                    }
                    else if(polygon_point.x == x_min)
                    {
                        if(polygon_point.y > y_max)
                        {
                            y_max = polygon_point.y;
                            current_start_polygon_idx = i;
                            current_start_point_idx = j;
                        }
                    }
                }
            }
        }

        if(true)
        {
            for(size_t i = 0; i < split_polygon_vec.size(); ++i)
            {
                const EasyPolygon2D &split_polygon = split_polygon_vec[i];
                std::cout << "current polygon " << i << " :" << std::endl <<
                  "points :" << std::endl;
                for(int j = 0; j < split_polygon.point_list.size(); ++j)
                {
                    std::cout << "\t" << j << " : [" <<
                      split_polygon.point_list[j].x << "," <<
                      split_polygon.point_list[j].y << "]" << std::endl;
                }
            }
            std::cout << "current intersection:" << std::endl;
            for(int i = 0; i < split_intersection_vec.size(); ++i)
            {
                std::cout << "points :" << std::endl;
                std::cout << "\t" << i << " : [" <<
                  split_intersection_vec[i].point.x << "," <<
                  split_intersection_vec[i].point.y << "]" << std::endl;
                std::cout << "idx :" << std::endl;
                for(int j = 0;
                    j < split_intersection_vec[i].polygon_point_idx_vec_pair_vec.size();
                    ++j)
                {
                    const auto &polygon_line_idx_vec_pair =
                      split_intersection_vec[i].polygon_point_idx_vec_pair_vec[j];
                    std::cout << "\tpolygon id : " <<
                      polygon_line_idx_vec_pair.first << std::endl;
                    std::cout << "\tpolygon line idx :";
                    for(int k = 0; k < polygon_line_idx_vec_pair.second.size(); ++k)
                    {
                        std::cout << polygon_line_idx_vec_pair.second[k] << " , ";
                    }
                    std::cout << std::endl;
                }
            }
        }
        

        int start_point_idx = current_start_point_idx;

        addNewPolygonPoint(
            split_polygon_vec,
            current_start_polygon_idx,
            current_start_point_idx,
            current_union_polygon,
            polygon_point_connected_vec_vec);

        std::cout << "current running at p" << current_start_polygon_idx << current_start_point_idx << std::endl;

        size_t intersection_idx;
        if(getIntersectionIdxOnPolygonPoint(
              split_polygon_vec,
              split_intersection_vec,
              current_start_polygon_idx,
              current_start_point_idx,
              intersection_idx))
        {
            updatePolygonIntersectionPointConnectedState(
                split_intersection_vec[intersection_idx],
                polygon_point_connected_vec_vec);
        }

        current_start_point_idx =
          (current_start_point_idx + 1) % split_polygon_vec[current_start_polygon_idx].point_list.size();

        std::cout << "current running at p" << current_start_polygon_idx << current_start_point_idx << std::endl;
        std::cout << "start running getUnionPolygonPoints" << std::endl;

        getUnionPolygonPoints(
            split_polygon_vec,
            split_intersection_vec,
            current_start_polygon_idx,
            start_point_idx,
            current_start_polygon_idx,
            current_start_point_idx,
            current_union_polygon,
            polygon_point_connected_vec_vec);

        updatePolygonNotIntersectionPointConnectedState(
            split_polygon_vec,
            current_union_polygon,
            polygon_point_connected_vec_vec);

        union_polygon_vec.emplace_back(current_union_polygon);

        std::cout << "current union_polygon:" << std::endl <<
          "points :" << std::endl;
        for(int i = 0; i < current_union_polygon.point_list.size(); ++i)
        {
            std::cout << "\t" << i << " : [" <<
              current_union_polygon.point_list[i].x << "," <<
              current_union_polygon.point_list[i].y << "]" << std::endl;
        }

        connected_polygon_point_num = 0;

        for(const std::vector<bool> &polygon_point_connected_vec :
            polygon_point_connected_vec_vec)
        {
            connected_polygon_point_num += count(
                polygon_point_connected_vec.cbegin(),
                polygon_point_connected_vec.cend(),
                true);
        }

        std::cout << "now still have :";
        for(size_t i = 0; i < polygon_point_connected_vec_vec.size(); ++i)
        {
            const auto &polygon_point_connected_vec = polygon_point_connected_vec_vec[i];
            for(size_t j = 0; j < polygon_point_connected_vec.size(); ++j)
            {
                if(!polygon_point_connected_vec[j])
                {
                    std::cout << " p" << i << j;
                }
            }
        }
        std::cout << " not connected!" << std::endl;

        std::cout << "connected_polygon_point_num = " << connected_polygon_point_num << std::endl;
        std::cout << "total_polygon_point_num = " << total_polygon_point_num << std::endl;
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

float EasyMask2D::getClockWiseAngle(
    const EasyLine2D &line_1,
    const EasyLine2D &line_2)
{
    float cross_value = cross(line_1, line_2);

    float anti_clock_small_angle = angle(line_1, line_2);

    if(cross_value > 0)
    {
        return 2.0 * M_PI - anti_clock_small_angle;
    }

    if(cross_value == 0)
    {
        if(line_1.x_diff != 0)
        {
            float x_diff_mul = line_1.x_diff * line_2.x_diff;

            if(x_diff_mul < 0)
            {
                return M_PI;
            }

            return 0;
        }

        if(line_1.y_diff != 0)
        {
            float y_diff_mul = line_1.y_diff * line_2.y_diff;

            if(y_diff_mul < 0)
            {
                return M_PI;
            }

            return 0;
        }

        return 0;
    }

    return - anti_clock_small_angle;
}

float EasyMask2D::getAntiClockWiseAngle(
    const EasyLine2D &line_1,
    const EasyLine2D &line_2)
{
    float cross_value = cross(line_1, line_2);

    float anti_clock_small_angle = angle(line_1, line_2);

    if(cross_value < 0)
    {
        return 2.0 * M_PI + anti_clock_small_angle;
    }

    if(cross_value == 0)
    {
        if(line_1.x_diff != 0)
        {
            float x_diff_mul = line_1.x_diff * line_2.x_diff;

            if(x_diff_mul < 0)
            {
                return M_PI;
            }

            return 0;
        }

        if(line_1.y_diff != 0)
        {
            float y_diff_mul = line_1.y_diff * line_2.y_diff;

            if(y_diff_mul < 0)
            {
                return M_PI;
            }

            return 0;
        }

        return 0;
    }

    return anti_clock_small_angle;
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
    float angle_value_sum_to_pi = fabs(angle_value_sum_to_0 - M_PI);
    float angle_value_sum_to_2pi = fabs(angle_value_sum - 2.0 * M_PI);

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
    std::vector<EasyPolygon2D> &polygon_vec,
    std::vector<EasyIntersection2D> &intersection_vec)
{
    intersection_vec.clear();

    if(polygon_vec.size() < 2)
    {
        return true;
    }

    for(size_t i = 1; i < polygon_vec.size(); ++i)
    {
        EasyPolygon2D &polygon_1 = polygon_vec[i];

        for(size_t j = 0; j < i; ++j)
        {
            EasyPolygon2D &polygon_2 = polygon_vec[j];

            for(size_t k = 0; k < polygon_1.point_list.size(); ++k)
            {
                EasyLine2D polygon_1_line;
                polygon_1_line.setPosition(
                    polygon_1.point_list[k],
                    polygon_1.point_list[(k + 1) % polygon_1.point_list.size()]);

                for(size_t l = 0; l < polygon_2.point_list.size(); ++l)
                {
                    EasyLine2D polygon_2_line;
                    polygon_2_line.setPosition(
                        polygon_2.point_list[l],
                        polygon_2.point_list[(l + 1) % polygon_2.point_list.size()]);

                    std::vector<EasyPoint2D> line_cross_point_vec;
                    getBoundedLineCrossPointVec(
                        polygon_1_line,
                        polygon_2_line,
                        line_cross_point_vec);

                    if(line_cross_point_vec.size() == 0)
                    {
                        continue;
                    }

                    for(const EasyPoint2D &line_cross_point : line_cross_point_vec)
                    {
                        bool cross_point_exist = false;

                        for(EasyIntersection2D &exist_intersection : intersection_vec)
                        {
                            if(isSamePoint(exist_intersection.point, line_cross_point))
                            {
                                exist_intersection.addPolygonPointIdx(i, k);
                                exist_intersection.addPolygonPointIdx(j, l);
                                cross_point_exist = true;

                                break;
                            }
                        }

                        if(!cross_point_exist)
                        {
                            EasyIntersection2D intersection;

                            intersection.setPosition(line_cross_point);

                            intersection.addPolygonPointIdx(i, k);
                            intersection.addPolygonPointIdx(j, l);

                            intersection_vec.emplace_back(intersection);
                        }
                    }
                }
            }
        }
    }

    return true;
}

bool EasyMask2D::getSortedIntersectionOnPolygonLine(
    const EasyPolygon2D &polygon,
    const std::vector<EasyIntersection2D> &intersection_vec,
    const size_t &polygon_idx,
    const size_t &point_idx,
    std::vector<size_t> &sorted_intersection_idx_on_polygon_line_vec)
{
    sorted_intersection_idx_on_polygon_line_vec.clear();

    if(intersection_vec.size() == 0)
    {
        return true;
    }

    std::vector<size_t> intersection_idx_on_polygon_line_vec;

    for(size_t i = 0; i < intersection_vec.size(); ++i)
    {
        const EasyIntersection2D &intersection = intersection_vec[i];

        if(intersection.haveThisPointIdx(polygon_idx, point_idx))
        {
            intersection_idx_on_polygon_line_vec.emplace_back(i);
        }
    }

    const EasyPoint2D &point_start = polygon.point_list[point_idx];
    const EasyPoint2D &point_end = polygon.point_list[(point_idx + 1) % polygon.point_list.size()];

    std::vector<float> intersection_to_point_start_diff_vec;
    intersection_to_point_start_diff_vec.resize(intersection_idx_on_polygon_line_vec.size());

    std::vector<bool> intersection_idx_selected_vec;
    intersection_idx_selected_vec.resize(intersection_idx_on_polygon_line_vec.size(), false);

    if(point_start.x != point_end.x)
    {
        float line_x_diff = point_end.x - point_start.x;

        for(size_t i = 0; i < intersection_idx_on_polygon_line_vec.size(); ++i)
        {
            intersection_to_point_start_diff_vec[i] =
              (intersection_vec[intersection_idx_on_polygon_line_vec[i]].point.x - point_start.x) /
              line_x_diff;
        }

        for(size_t i = 0; i < intersection_idx_on_polygon_line_vec.size(); ++i)
        {
            float min_dist = -1;
            int min_dist_intersection_idx = -1;

            for(size_t j = 0; j < intersection_idx_on_polygon_line_vec.size(); ++j)
            {
                if(intersection_idx_selected_vec[j])
                {
                    continue;
                }

                if(min_dist_intersection_idx == -1 ||
                    intersection_to_point_start_diff_vec[j] < min_dist)
                {
                    min_dist = intersection_to_point_start_diff_vec[j];
                    min_dist_intersection_idx = j;
                }
            }

            intersection_idx_selected_vec[min_dist_intersection_idx] = true;

            sorted_intersection_idx_on_polygon_line_vec.emplace_back(
                intersection_idx_on_polygon_line_vec[min_dist_intersection_idx]);
        }

        return true;
    }

    float line_y_diff = point_end.y - point_start.y;

    for(size_t i = 0; i < intersection_idx_on_polygon_line_vec.size(); ++i)
    {
        intersection_to_point_start_diff_vec[i] =
          (intersection_vec[intersection_idx_on_polygon_line_vec[i]].point.y - point_start.y) /
          line_y_diff;
    }

    for(size_t i = 0; i < intersection_idx_on_polygon_line_vec.size(); ++i)
    {
        float min_dist = -1;
        int min_dist_intersection_idx = -1;

        for(size_t j = 0; j < intersection_idx_on_polygon_line_vec.size(); ++j)
        {
            if(intersection_idx_selected_vec[j])
            {
                continue;
            }

            if(min_dist_intersection_idx == -1 ||
                intersection_to_point_start_diff_vec[j] < min_dist)
            {
                min_dist = intersection_to_point_start_diff_vec[j];
                min_dist_intersection_idx = j;
            }
        }

        intersection_idx_selected_vec[min_dist_intersection_idx] = true;

        sorted_intersection_idx_on_polygon_line_vec.emplace_back(
            intersection_idx_on_polygon_line_vec[min_dist_intersection_idx]);
    }

    return true;
}

bool EasyMask2D::getSplitPolygonAndIntersectionPosition(
    const EasyPolygon2D &polygon,
    const std::vector<EasyIntersection2D> &intersection_vec,
    const size_t &polygon_idx,
    EasyPolygon2D &split_polygon,
    std::vector<std::pair<size_t, size_t>> &intersection_idx_polygon_point_idx_pair_vec)
{
    intersection_idx_polygon_point_idx_pair_vec.clear();

    if(intersection_vec.size() == 0)
    {
        return true;
    }

    size_t current_new_point_idx = 0;

    for(size_t i = 0; i < polygon.point_list.size(); ++i)
    {
        split_polygon.addPoint(polygon.point_list[i]);

        ++current_new_point_idx;

        std::vector<size_t> sorted_intersection_idx_on_polygon_line_vec;
        getSortedIntersectionOnPolygonLine(
            polygon,
            intersection_vec,
            polygon_idx,
            i,
            sorted_intersection_idx_on_polygon_line_vec);

        if(sorted_intersection_idx_on_polygon_line_vec.size() == 0)
        {
            continue;
        }

        for(const size_t &sorted_intersection_idx : sorted_intersection_idx_on_polygon_line_vec)
        {
            if(isSamePoint(
                  polygon.point_list[i],
                  intersection_vec[sorted_intersection_idx].point))
            {
                std::pair<size_t, size_t> intersection_idx_polygon_point_idx_pair;
                intersection_idx_polygon_point_idx_pair.first = sorted_intersection_idx;
                intersection_idx_polygon_point_idx_pair.second = current_new_point_idx - 1;
                intersection_idx_polygon_point_idx_pair_vec.emplace_back(
                    intersection_idx_polygon_point_idx_pair);

                continue;
            }

            if(isSamePoint(
                  polygon.point_list[(i + 1) % polygon.point_list.size()],
                  intersection_vec[sorted_intersection_idx].point))
            {
                continue;
            }

            split_polygon.addPoint(intersection_vec[sorted_intersection_idx].point);

            std::pair<size_t, size_t> intersection_idx_polygon_point_idx_pair;
            intersection_idx_polygon_point_idx_pair.first = sorted_intersection_idx;
            intersection_idx_polygon_point_idx_pair.second = current_new_point_idx;
            intersection_idx_polygon_point_idx_pair_vec.emplace_back(
                intersection_idx_polygon_point_idx_pair);

            ++current_new_point_idx;
        }
    }

    return true;
}

bool EasyMask2D::splitPolygonsByIntersection(
    const std::vector<EasyIntersection2D> &intersection_vec,
    const std::vector<EasyPolygon2D> &polygon_vec,
    std::vector<EasyIntersection2D> &split_intersection_vec,
    std::vector<EasyPolygon2D> &split_polygon_vec)
{
    split_intersection_vec.clear();
    split_polygon_vec.clear();

    if(intersection_vec.size() == 0)
    {
        split_polygon_vec = polygon_vec;
        return true;
    }

    for(const EasyIntersection2D &intersection : intersection_vec)
    {
        const EasyPoint2D &intersection_point = intersection.point;

        EasyIntersection2D empty_split_intersection;
        empty_split_intersection.setPosition(intersection_point);
        split_intersection_vec.emplace_back(empty_split_intersection);
    }

    for(size_t i = 0; i < polygon_vec.size(); ++i)
    {
        const EasyPolygon2D &polygon = polygon_vec[i];

        EasyPolygon2D split_polygon;
        std::vector<std::pair<size_t, size_t>> intersection_idx_polygon_point_idx_pair_vec;

        getSplitPolygonAndIntersectionPosition(
            polygon,
            intersection_vec,
            i,
            split_polygon,
            intersection_idx_polygon_point_idx_pair_vec);

        split_polygon_vec.emplace_back(split_polygon);

        for(const std::pair<size_t, size_t> &intersection_idx_polygon_point_idx_pair :
            intersection_idx_polygon_point_idx_pair_vec)
        {
            split_intersection_vec[intersection_idx_polygon_point_idx_pair.first].addPolygonPointIdx(
                i,
                intersection_idx_polygon_point_idx_pair.second);
        }
    }

    return true;
}

bool EasyMask2D::getIntersectionIdxOnPolygonPoint(
    const std::vector<EasyPolygon2D> &polygon_vec,
    const std::vector<EasyIntersection2D> &intersection_vec,
    const size_t &polygon_idx,
    const size_t &point_idx,
    size_t &intersection_idx)
{
    if(intersection_vec.size() == 0)
    {
        return false;
    }

    for(size_t i = 0; i < intersection_vec.size(); ++i)
    {
        if(isSamePoint(intersection_vec[i].point, polygon_vec[polygon_idx].point_list[point_idx]))
        {
            intersection_idx = i;

            return true;
        }
    }

    return false;
}

bool EasyMask2D::updatePolygonIntersectionPointConnectedState(
    const EasyIntersection2D &intersection,
    std::vector<std::vector<bool>> &polygon_point_connected_vec_vec)
{
    for(const std::pair<size_t, std::vector<size_t>> &polygon_point_idx_vec_pair :
        intersection.polygon_point_idx_vec_pair_vec)
    {
        for(const size_t polygon_point_idx : polygon_point_idx_vec_pair.second)
        {
            polygon_point_connected_vec_vec[polygon_point_idx_vec_pair.first][polygon_point_idx] = true;
        }
    }

    return true;
}

bool EasyMask2D::updatePolygonNotIntersectionPointConnectedState(
    const std::vector<EasyPolygon2D> &polygon_vec,
    const EasyPolygon2D &union_polygon,
    std::vector<std::vector<bool>> &polygon_point_connected_vec_vec)
{
    for(size_t i = 0; i < polygon_vec.size(); ++i)
    {
        const EasyPolygon2D &polygon = polygon_vec[i];

        for(size_t j = 0; j < polygon.point_list.size(); ++j)
        {
            if(polygon_point_connected_vec_vec[i][j])
            {
                continue;
            }

            const EasyPoint2D &polygon_point = polygon.point_list[j];

            if(isPointInPolygon(polygon_point, union_polygon))
            {
                polygon_point_connected_vec_vec[i][j] = true;
            }
        }
    }

    return true;
}

bool EasyMask2D::addNewPolygonPoint(
    const std::vector<EasyPolygon2D> &polygon_vec,
    const size_t &polygon_idx,
    const size_t &start_point_idx,
    EasyPolygon2D &union_polygon,
    std::vector<std::vector<bool>> &polygon_point_connected_vec_vec)
{
    if(polygon_idx >= polygon_vec.size())
    {
        std::cout << "EasyMask2D::addNewPolygonPoint : polygon_idx out of range!" << std::endl;
        return false;
    }

    union_polygon.addPoint(
        polygon_vec[polygon_idx].point_list[start_point_idx]);

    polygon_point_connected_vec_vec[polygon_idx][start_point_idx] = true;

    return true;
}

bool EasyMask2D::getMinimalAnglePolygonPointIdx(
    const std::vector<EasyPolygon2D> &polygon_vec,
    const std::vector<EasyIntersection2D> &intersection_vec,
    const size_t &polygon_idx,
    const size_t &start_point_idx,
    const size_t &start_intersection_idx,
    std::pair<size_t, size_t> &minimal_angle_polygon_point_idx)
{
    if(intersection_vec.size() == 0)
    {
        std::cout << "EasyMask2D::getMinimalAnglePolygonPointIdx : intersection is empty!" << std::endl;
        return false;
    }

    float minimal_angle = 2.0 * M_PI;

    const size_t polygon_prev_point_idx =
      (start_point_idx - 1 + polygon_vec[polygon_idx].point_list.size()) %
      polygon_vec[polygon_idx].point_list.size();

    EasyLine2D line_intersection_to_polygon_prev_point;
    line_intersection_to_polygon_prev_point.setPosition(
        intersection_vec[start_intersection_idx].point,
        polygon_vec[polygon_idx].point_list[polygon_prev_point_idx]);

    for(const std::pair<size_t, std::vector<size_t>> &polygon_point_idx_vec_pair :
        intersection_vec[start_intersection_idx].polygon_point_idx_vec_pair_vec)
    {
        for(const size_t &polygon_point_idx : polygon_point_idx_vec_pair.second)
        {
            const size_t polygon_next_point_idx =
              (polygon_point_idx + 1) %
              polygon_vec[polygon_point_idx_vec_pair.first].point_list.size();

            EasyLine2D line_intersection_to_polygon_next_point;
            line_intersection_to_polygon_next_point.setPosition(
                intersection_vec[start_intersection_idx].point,
                polygon_vec[polygon_point_idx_vec_pair.first].point_list[polygon_next_point_idx]);

            float current_angle = getAntiClockWiseAngle(
                line_intersection_to_polygon_prev_point,
                line_intersection_to_polygon_next_point);

            if(current_angle == 0)
            {
                continue;
            }

            if(current_angle < minimal_angle)
            {
                minimal_angle = current_angle;
                minimal_angle_polygon_point_idx.first = polygon_point_idx_vec_pair.first;
                minimal_angle_polygon_point_idx.second = polygon_next_point_idx;
            }
        }
    }

    if(minimal_angle == 2.0 * M_PI)
    {
        std::cout << "no new polygon point found!" << std::endl;
        return false;
    }

    return true;
}

bool EasyMask2D::getUnionPolygonPoints(
    const std::vector<EasyPolygon2D> &polygon_vec,
    const std::vector<EasyIntersection2D> &intersection_vec,
    const size_t &start_polygon_idx,
    const size_t &start_point_idx,
    const size_t &current_start_polygon_idx,
    const size_t &current_start_point_idx,
    EasyPolygon2D &union_polygon,
    std::vector<std::vector<bool>> &polygon_point_connected_vec_vec)
{
    std::cout << "current running at p" << current_start_polygon_idx << current_start_point_idx << std::endl;
    size_t connected_polygon_point_num = 0;

    for(const std::vector<bool> &polygon_point_connected_vec :
        polygon_point_connected_vec_vec)
    {
        connected_polygon_point_num += count(
            polygon_point_connected_vec.cbegin(),
            polygon_point_connected_vec.cend(),
            true);
    }
    std::cout << "connected_polygon_point_num_ = " << connected_polygon_point_num_ << std::endl;
    std::cout << "connected_polygon_point_num = " << connected_polygon_point_num << std::endl;
    if(connected_polygon_point_num_ == connected_polygon_point_num)
    {
        std::cout << "force quit" << std::endl;
        exit(0);
    }
    connected_polygon_point_num_ = connected_polygon_point_num;

    if(current_start_polygon_idx == start_polygon_idx &&
        current_start_point_idx == start_point_idx)
    {
        return true;
    }

    size_t new_start_polygon_idx = current_start_polygon_idx;
    size_t new_start_point_idx = current_start_point_idx;

    addNewPolygonPoint(
        polygon_vec,
        new_start_polygon_idx,
        new_start_point_idx,
        union_polygon,
        polygon_point_connected_vec_vec);

    size_t intersection_idx;

    if(getIntersectionIdxOnPolygonPoint(
          polygon_vec,
          intersection_vec,
          new_start_polygon_idx,
          new_start_point_idx,
          intersection_idx))
    {
        updatePolygonIntersectionPointConnectedState(
            intersection_vec[intersection_idx],
            polygon_point_connected_vec_vec);

        std::pair<size_t, size_t> minimal_angle_polygon_point_idx_pair;

        getMinimalAnglePolygonPointIdx(
            polygon_vec,
            intersection_vec,
            new_start_polygon_idx,
            new_start_point_idx,
            intersection_idx,
            minimal_angle_polygon_point_idx_pair);

        new_start_polygon_idx = minimal_angle_polygon_point_idx_pair.first;
        new_start_point_idx = minimal_angle_polygon_point_idx_pair.second;

        return getUnionPolygonPoints(
            polygon_vec,
            intersection_vec,
            start_polygon_idx,
            start_point_idx,
            new_start_polygon_idx,
            new_start_point_idx,
            union_polygon,
            polygon_point_connected_vec_vec);
    }

    new_start_point_idx =
      (new_start_point_idx + 1) % polygon_vec[new_start_polygon_idx].point_list.size();

    return getUnionPolygonPoints(
        polygon_vec,
        intersection_vec,
        start_polygon_idx,
        start_point_idx,
        new_start_polygon_idx,
        new_start_point_idx,
        union_polygon,
        polygon_point_connected_vec_vec);
}

