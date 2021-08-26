#include "EasyPolygon.h"

bool EasyPoint2D::setPosition(
    const float &position_x,
    const float &position_y)
{
    x = position_x;
    y = position_y;

    return true;
}

bool EasyRect2D::setPosition(
    const float &position_x_min,
    const float &position_y_min,
    const float &position_x_max,
    const float &position_y_max)
{
    x_min = position_x_min;
    y_min = position_y_min;
    x_max = position_x_max;
    y_max = position_y_max;

    x_diff = x_max - x_min;
    y_diff = y_max - y_min;

    return true;
}

bool EasyLine2D::setPosition(
    const float &position_x_1,
    const float &position_y_1,
    const float &position_x_2,
    const float &position_y_2)
{
    point_1.x = position_x_1;
    point_1.y = position_y_1;
    point_2.x = position_x_2;
    point_2.y = position_y_2;

    x_diff = point_2.x - point_1.x;
    y_diff = point_2.y - point_1.y;

    float x_min;
    float x_max;
    float y_min;
    float y_max;

    if(x_diff > 0)
    {
        x_min = point_1.x;
        x_max = point_2.x;
    }
    else
    {
        x_min = point_2.x;
        x_max = point_1.x;
    }

    if(y_diff > 0)
    {
        y_min = point_1.y;
        y_max = point_2.y;
    }
    else
    {
        y_min = point_2.y;
        y_max = point_1.y;
    }

    rect.setPosition(x_min, y_min, x_max, y_max);

    return true;
}

bool EasyPolygon::insertPoint(
    const EasyPoint2D &point_2d,
    const int &insert_idx)
{
    if(insert_idx == -1)
    {
        point_list.emplace_back(point_2d);

        return true;
    }

    if(insert_idx < 0 || insert_idx > point_list.size() - 1)
    {
        std::cout << "insert point to polygon failed!" << std::endl;
        return false;
    }

    point_list.insert(point_list.begin() + insert_idx, point_2d);

    return true;
}

bool EasyPolygon::removePoint(
    const int &remove_idx)
{
    if(remove_idx < 0 || remove_idx > point_list.size() - 1)
    {
        std::cout << "remove point from polygon failed!" << std::endl;
        return false;
    }

    point_list.erase(point_list.begin() + remove_idx);

    return true;
}

bool EasyPolygon::isClockWise()
{
    if(getPolygonArea() > 0)
    {
        return true;
    }

    return false;
}

bool EasyPolygon::setClockWise()
{
    if(!isClockWise())
    {
        std::reverse(point_list.begin(), point_list.end());
    }

    return true;
}

float EasyPolygon::getPolygonArea()
{
    if(point_list.size() < 3)
    {
        return 0;
    }

    float area = 0;
    
    for(int i = 0; i < point_list.size(); ++i)
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

bool EasyPolygon::getPolygonRect(
    EasyRect2D &rect_2d)
{
    if(point_list.size() == 0)
    {
        std::cout << "EasyPolygon::getPolygonRect : no point found!" << std::endl;
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

bool EasyPolygon::isPointInPolygon(
    const EasyPoint2D &point)
{
    float angle_value_sum = 0;

    for(int i = 0; i < point_list.size(); ++i)
    {
        int next_point_idx = i + 1;
        if(i == point_list.size() - 1)
        {
            next_point_idx = 0;
        }

        angle_value_sum += angle(
            point_list[i],
            point,
            point_list[next_point_idx],
            point);
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

bool EasyPolygon::getUnionPolygon(
    const EasyPolygon &polygon_1,
    const EasyPolygon &polygon_2,
    EasyPolygon &union_polygon)
{
    return true;
}

float EasyPolygon::dot(
    const float &x_1,
    const float &y_1,
    const float &x_2,
    const float &y_2)
{
    return x_1 * x_2 + y_1 * y_2;
}

float EasyPolygon::dot(
    const EasyLine2D &line_1,
    const EasyLine2D &line_2)
{
    return dot(line_1.x_diff, line_1.y_diff, line_2.x_diff, line_2.y_diff);
}

float EasyPolygon::cross(
    const float &x_1,
    const float &y_1,
    const float &x_2,
    const float &y_2)
{
    return x_1 * y_2 - x_2 * y_1;
}

float EasyPolygon::cross(
    const EasyLine2D &line_1,
    const EasyLine2D &line_2)
{
    return cross(line_1.x_diff, line_1.y_diff, line_2.x_diff, line_2.y_diff);
}

float EasyPolygon::lineLength2(
    const EasyLine2D &line)
{
    float length_2 = pow(line.x_diff, 2) + pow(line.y_diff, 2);

    return length_2;
}

float EasyPolygon::lineLength(
    const EasyLine2D &line)
{
    return sqrt(lineLength2(line));
}

float EasyPolygon::angle(
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

bool EasyPolygon::isPointInRect(
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

bool EasyPolygon::isPointInLineRect(
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

bool EasyPolygon::isRectCross(
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

bool EasyPolygon::isRectCross(
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

bool EasyPolygon::isLineCross(
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
        line_2.point_2.x,
        line_2.point_2.y,
        line_1.point_1.x,
        line_1.point_1.y);
    line_22_to_12.setPosition(
        line_2.point_2.x,
        line_2.point_2.y,
        line_1.point_2.x,
        line_1.point_2.y);

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

bool EasyPolygon::isLineParallel(
    const EasyLine2D &line_1,
    const EasyLine2D &line_2)
{
    if(cross(line_1, line_2) == 0)
    {
        return true;
    }

    return false;
}

bool EasyPolygon::isPointOnOpenBoundedLine(
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

bool EasyPolygon::isPolygonCross(
    EasyPolygon &polygon)
{
    EasyRect2D self_pylygon_rect;
    EasyRect2D polygon_rect;

    getPolygonRect(self_pylygon_rect);
    polygon.getPolygonRect(polygon_rect);

    if(!isRectCross(self_pylygon_rect, polygon_rect))
    {
        return false;
    }

    for(const EasyPoint2D &point : point_list)
    {
        if(polygon.isPointInPolygon(point))
        {
            return true;
        }
    }

    return false;
}

bool EasyPolygon::getLineCrossPoint(
    const EasyLine2D &line_1,
    const EasyLine2D &line_2,
    EasyPoint2D &line_cross_point)
{
    float line_cross = cross(line_1, line_2);

    if(line_cross == 0)
    {
        std::cout << "EasyPolygon::getLineCrossPoint : lines are parallel!" << std::endl;
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

bool EasyPolygon::getBoundedLineCrossPoints(
    const EasyLine2D &line_1,
    const EasyLine2D &line_2,
    std::vector<EasyPoint2D> &line_cross_point_vec)
{
    line_cross_point_vec.clear();

    if(!isLineCross(line_1, line_2))
    {
        std::cout << "EasyPolygon::getBoundedLineCrossPoints : line not crossed!" << std::endl;
        return false;
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

bool EasyPolygon::isSamePoint(
    const EasyPoint2D &point_1,
    const EasyPoint2D &point_2)
{
    if(point_1.x == point_2.x && point_1.y == point_2.y)
    {
        return true;
    }

    return false;
}

bool EasyPolygon::getPolygonIntersection(
    const EasyPolygon &polygon_1,
    const EasyPolygon &polygon_2,
    std::vector<EasyPoint2D> &intersect_point_vec)
{
    for(int i = 0; i < polygon_1.point_list.size(); ++i)
    {
        int next_point_idx_1 = i + 1;
        if(i == polygon_1.point_list.size() - 1)
        {
            next_point_idx_1 = 0;
        }

        for(int j = 0; j < polygon_2.point_list.size(); ++j)
        {
            int next_point_idx_2 = j + 1;
            if(j == polygon_2.point_list.size())
            {
                next_point_idx_2 = 0;
            }
        }
    }

    return true;
}

