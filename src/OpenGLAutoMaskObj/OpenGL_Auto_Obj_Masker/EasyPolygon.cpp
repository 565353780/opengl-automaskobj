#include "EasyPolygon.h"

bool EasyPoint2D::setPosition(
    const float &position_x,
    const float &position_y)
{
    x = position_x;
    y = position_y;

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
    std::vector<float> &polygon_rect)
{
    polygon_rect.resize(4);

    if(point_list.size() == 0)
    {
        std::cout << "EasyPolygon::getPolygonRect : no point found!" << std::endl;
        return false;
    }

    float x_min = point_list[0].x;
    float x_max = x_min;
    float y_min = point_list[0].y;
    float y_max = y_min;

    for(const EasyPoint2D &point : point_list)
    {
        x_min = fmin(x_min, point.x);
        x_max = fmax(x_max, point.x);
        y_min = fmin(y_min, point.y);
        y_max = fmax(y_max, point.y);
    }

    polygon_rect[0] = x_min;
    polygon_rect[1] = y_min;
    polygon_rect[2] = x_max;
    polygon_rect[3] = y_max;

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
    const EasyPoint2D &line_1_point_1,
    const EasyPoint2D &line_1_point_2,
    const EasyPoint2D &line_2_point_1,
    const EasyPoint2D &line_2_point_2)
{
    float line_1_x_diff = line_1_point_2.x - line_1_point_1.x;
    float line_1_y_diff = line_1_point_2.y - line_1_point_1.y;
    float line_2_x_diff = line_2_point_2.x - line_2_point_1.x;
    float line_2_y_diff = line_2_point_2.y - line_2_point_1.y;

    return dot(line_1_x_diff, line_1_y_diff, line_2_x_diff, line_2_y_diff);
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
    const EasyPoint2D &line_1_point_1,
    const EasyPoint2D &line_1_point_2,
    const EasyPoint2D &line_2_point_1,
    const EasyPoint2D &line_2_point_2)
{
    float line_1_x_diff = line_1_point_2.x - line_1_point_1.x;
    float line_1_y_diff = line_1_point_2.y - line_1_point_1.y;
    float line_2_x_diff = line_2_point_2.x - line_2_point_1.x;
    float line_2_y_diff = line_2_point_2.y - line_2_point_1.y;

    return cross(line_1_x_diff, line_1_y_diff, line_2_x_diff, line_2_y_diff);
}

float EasyPolygon::lineLength2(
    const EasyPoint2D &line_point_1,
    const EasyPoint2D &line_point_2)
{
    float length_2 =
      pow(line_point_1.x - line_point_2.x, 2) +
      pow(line_point_1.y - line_point_2.y, 2);

    return length_2;
}

float EasyPolygon::lineLength(
    const EasyPoint2D &line_point_1,
    const EasyPoint2D &line_point_2)
{
    return sqrt(lineLength2(line_point_1, line_point_2));
}

float EasyPolygon::angle(
    const EasyPoint2D &line_1_point_1,
    const EasyPoint2D &line_1_point_2,
    const EasyPoint2D &line_2_point_1,
    const EasyPoint2D &line_2_point_2)
{
    int sign = 1;

    if(cross(line_1_point_1, line_1_point_2, line_2_point_1, line_2_point_2) == 0)
    {
        return 0;
    }

    if(cross(line_1_point_1, line_1_point_2, line_2_point_1, line_2_point_2) < 0)
    {
        sign = -1;
    }

    float dot_value = dot(line_1_point_1, line_1_point_2, line_2_point_1, line_2_point_2);

    float line_1_length = lineLength(line_1_point_1, line_1_point_2);
    float line_2_length = lineLength(line_2_point_1, line_2_point_2);

    float cos_value = dot_value / (line_1_length * line_2_length);

    float angle_value = sign * acos(cos_value);

    return angle_value;
}

bool EasyPolygon::isPointInRect(
    const float &point_x,
    const float &point_y,
    const float &rect_x_min,
    const float &rect_y_min,
    const float &rect_x_max,
    const float &rect_y_max)
{
    if(point_x < rect_x_min)
    {
        return false;
    }

    if(point_x > rect_x_max)
    {
        return false;
    }

    if(point_y < rect_y_min)
    {
        return false;
    }

    if(point_y > rect_y_max)
    {
        return false;
    }

    return true;
}

bool EasyPolygon::isPointInRect(
    const EasyPoint2D &point,
    const float &rect_x_min,
    const float &rect_y_min,
    const float &rect_x_max,
    const float &rect_y_max)
{
    return isPointInRect(point.x, point.y, rect_x_min, rect_y_min, rect_x_max, rect_y_max);
}

bool EasyPolygon::isPointInLineRect(
    const EasyPoint2D &point,
    const EasyPoint2D &line_point_1,
    const EasyPoint2D &line_point_2)
{
    float rect_x_min = fmin(line_point_1.x, line_point_2.x);
    if(point.x < rect_x_min)
    {
        return false;
    }

    float rect_x_max = fmax(line_point_1.x, line_point_2.x);
    if(point.x > rect_x_max)
    {
        return false;
    }

    float rect_y_min = fmin(line_point_1.y, line_point_2.y);
    if(point.y < rect_y_min)
    {
        return false;
    }

    float rect_y_max = fmax(line_point_1.y, line_point_2.y);
    if(point.y > rect_y_max)
    {
        return false;
    }

    return true;
}

bool EasyPolygon::isRectCross(
    const EasyPoint2D &line_1_point_1,
    const EasyPoint2D &line_1_point_2,
    const EasyPoint2D &line_2_point_1,
    const EasyPoint2D &line_2_point_2)
{
    float line_1_x_max = fmax(line_1_point_1.x, line_1_point_2.x);
    float line_2_x_min = fmin(line_2_point_1.x, line_2_point_2.x);
    if(line_1_x_max < line_2_x_min)
    {
        return false;
    }

    float line_1_x_min = fmin(line_1_point_1.x, line_1_point_2.x);
    float line_2_x_max = fmax(line_2_point_1.x, line_2_point_2.x);
    if(line_1_x_min > line_2_x_max)
    {
        return false;
    }

    float line_1_y_max = fmax(line_1_point_1.y, line_1_point_2.y);
    float line_2_y_min = fmin(line_2_point_1.y, line_2_point_2.y);
    if(line_1_y_max < line_2_y_min)
    {
        return false;
    }

    float line_1_y_min = fmin(line_1_point_1.y, line_1_point_2.y);
    float line_2_y_max = fmax(line_2_point_1.y, line_2_point_2.y);
    if(line_1_y_min > line_2_y_max)
    {
        return false;
    }

    return true;
}

bool EasyPolygon::isLineCross(
    const EasyPoint2D &line_1_point_1,
    const EasyPoint2D &line_1_point_2,
    const EasyPoint2D &line_2_point_1,
    const EasyPoint2D &line_2_point_2)
{
    if(!isRectCross(line_1_point_1, line_1_point_2, line_2_point_1, line_2_point_2))
    {
        return false;
    }

    float line_1_point_1_cross_line_2 =
      cross(line_2_point_2, line_1_point_1, line_2_point_2, line_2_point_1);
    float line_1_point_2_corss_line_2 =
      cross(line_2_point_2, line_1_point_2, line_2_point_2, line_2_point_1);

    if(line_1_point_1_cross_line_2 == 0)
    {
        if(line_1_point_2_corss_line_2 == 0)
        {
            return true;
        }

        return isPointInLineRect(line_1_point_1, line_2_point_1, line_2_point_2);
    }

    if(line_1_point_2_corss_line_2 == 0)
    {
        return isPointInLineRect(line_1_point_2, line_2_point_1, line_2_point_2);
    }

    if(line_1_point_1_cross_line_2 * line_1_point_2_corss_line_2 < 0)
    {
        return true;
    }

    return false;
}

bool EasyPolygon::isLineParallel(
    const float &x_1,
    const float &y_1,
    const float &x_2,
    const float &y_2)
{
    if(cross(x_1, y_1, x_2, y_2) == 0)
    {
        return true;
    }

    return false;
}


bool EasyPolygon::isLineParallel(
    const EasyPoint2D &line_1_point_1,
    const EasyPoint2D &line_1_point_2,
    const EasyPoint2D &line_2_point_1,
    const EasyPoint2D &line_2_point_2)
{
    float line_1_x_diff = line_1_point_2.x - line_1_point_1.x;
    float line_1_y_diff = line_1_point_2.y - line_1_point_1.y;
    float line_2_x_diff = line_2_point_2.x - line_2_point_1.x;
    float line_2_y_diff = line_2_point_2.y - line_2_point_1.y;

    return isLineParallel(line_1_x_diff, line_1_y_diff, line_2_x_diff, line_2_y_diff);
}

bool EasyPolygon::isPointOnOpenBoundedLine(
    const EasyPoint2D &point,
    const EasyPoint2D &line_point_1,
    const EasyPoint2D &line_point_2)
{
    if(line_point_1.x != line_point_2.x)
    {
        float x_min = fmin(line_point_1.x, line_point_2.x);
        float x_max = fmax(line_point_1.x, line_point_2.x);
        if(point.x > x_min && point.x < x_max)
        {
            return true;
        }
    }
    else if(line_point_1.y != line_point_2.y)
    {
        float y_min = fmin(line_point_1.y, line_point_2.y);
        float y_max = fmax(line_point_1.y, line_point_2.y);
        if(point.y > y_min && point.y < y_max)
        {
            return true;
        }
    }

    return false;
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

bool EasyPolygon::isPolygonCross(
    EasyPolygon &polygon)
{
    std::vector<float> self_pylygon_rect;
    std::vector<float> polygon_rect;

    getPolygonRect(self_pylygon_rect);
    polygon.getPolygonRect(polygon_rect);

    EasyPoint2D self_polygon_rect_point_1;
    self_polygon_rect_point_1.setPosition(self_pylygon_rect[0], self_pylygon_rect[1]);
    EasyPoint2D self_polygon_rect_point_2;
    self_polygon_rect_point_2.setPosition(self_pylygon_rect[2], self_pylygon_rect[3]);
    EasyPoint2D polygon_rect_point_1;
    polygon_rect_point_1.setPosition(polygon_rect[0], polygon_rect[1]);
    EasyPoint2D polygon_rect_point_2;
    polygon_rect_point_2.setPosition(polygon_rect[2], polygon_rect[3]);

    if(isRectCross(
          self_polygon_rect_point_1,
          self_polygon_rect_point_2,
          polygon_rect_point_1,
          polygon_rect_point_2))
    {
    }

    return true;
}

bool EasyPolygon::getLineCrossPoint(
    const EasyPoint2D &line_1_point_1,
    const EasyPoint2D &line_1_point_2,
    const EasyPoint2D &line_2_point_1,
    const EasyPoint2D &line_2_point_2,
    EasyPoint2D &line_cross_point)
{
    float line_cross = cross(line_1_point_1, line_1_point_2, line_2_point_1, line_2_point_2);

    if(line_cross == 0)
    {
        std::cout << "EasyPolygon::getLineCrossPoint : lines are parallel!" << std::endl;
        return false;
    }

    float line_1_weight =
      (line_2_point_2.y - line_2_point_1.y) * line_2_point_1.x +
      (line_2_point_1.x - line_2_point_2.x) * line_2_point_1.y;

    float line_2_weight =
      (line_1_point_2.y - line_1_point_1.y) * line_1_point_1.x +
      (line_1_point_1.x - line_1_point_2.x) * line_1_point_1.y;

    float line_cross_point_x_weight =
      line_1_weight * (line_1_point_2.x - line_1_point_1.x) -
      line_2_weight * (line_2_point_2.x - line_2_point_1.x);

    float line_cross_point_y_weight =
      line_1_weight * (line_1_point_2.y - line_1_point_1.y) -
      line_2_weight * (line_2_point_2.y - line_2_point_1.y);

    line_cross_point.x = line_cross_point_x_weight / line_cross;
    line_cross_point.y = line_cross_point_y_weight / line_cross;

    return true;
}

bool EasyPolygon::getBoundedLineCrossPoints(
    const EasyPoint2D &line_1_point_1,
    const EasyPoint2D &line_1_point_2,
    const EasyPoint2D &line_2_point_1,
    const EasyPoint2D &line_2_point_2,
    std::vector<EasyPoint2D> &line_cross_point_vec)
{
    line_cross_point_vec.clear();

    if(!isLineCross(line_1_point_1, line_1_point_2, line_2_point_1, line_2_point_2))
    {
        std::cout << "EasyPolygon::getBoundedLineCrossPoints : line not crossed!" << std::endl;
        return false;
    }

    if(isLineParallel(line_1_point_1, line_1_point_2, line_2_point_1, line_2_point_2))
    {
        if(isSamePoint(line_1_point_1, line_2_point_1))
        {
            line_cross_point_vec.emplace_back(line_1_point_1);

            if(isSamePoint(line_1_point_2, line_2_point_2))
            {
                line_cross_point_vec.emplace_back(line_1_point_2);
            }
        }
        else if(isSamePoint(line_1_point_2, line_2_point_1))
        {
            line_cross_point_vec.emplace_back(line_1_point_2);

            if(isSamePoint(line_1_point_1, line_2_point_2))
            {
                line_cross_point_vec.emplace_back(line_1_point_1);
            }
        }

        if(isPointOnOpenBoundedLine(line_1_point_1, line_2_point_1, line_2_point_2))
        {
            line_cross_point_vec.emplace_back(line_1_point_1);
        }
        if(isPointOnOpenBoundedLine(line_1_point_2, line_2_point_1, line_2_point_2))
        {
            line_cross_point_vec.emplace_back(line_1_point_2);
        }
        if(isPointOnOpenBoundedLine(line_2_point_1, line_1_point_1, line_1_point_2))
        {
            line_cross_point_vec.emplace_back(line_2_point_1);
        }
        if(isPointOnOpenBoundedLine(line_2_point_2, line_1_point_1, line_1_point_2))
        {
            line_cross_point_vec.emplace_back(line_2_point_2);
        }

        return true;
    }

    EasyPoint2D line_cross_point;

    getLineCrossPoint(line_1_point_1, line_1_point_2, line_2_point_1, line_2_point_2, line_cross_point);

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

