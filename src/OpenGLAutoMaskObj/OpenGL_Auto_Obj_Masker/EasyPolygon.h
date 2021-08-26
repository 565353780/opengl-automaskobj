#include <iostream>
#include <vector>
#include <algorithm>
#include <stdlib.h>
#include <cmath>

class EasyPoint2D
{
public:
    EasyPoint2D()
    {
    }

    bool setPosition(
        const float &position_x,
        const float &position_y);

    float x;
    float y;
};

class EasyRect2D
{
public:
    EasyRect2D()
    {
    }

    bool setPosition(
        const float &x_min,
        const float &y_min,
        const float &x_max,
        const float &y_max);

    float x_min;
    float y_min;
    float x_max;
    float y_max;
};

class EasyPolygon
{
public:
    EasyPolygon()
    {
    }

    bool insertPoint(
        const EasyPoint2D &point_2d,
        const int &insert_idx=-1);

    bool removePoint(
        const int &remove_idx);

    bool isClockWise();

    bool setClockWise();

    float getPolygonArea();

    bool getPolygonRect(
        std::vector<float> &polygon_rect);

    bool getUnionPolygon(
        const EasyPolygon &polygon_1,
        const EasyPolygon &polygon_2,
        EasyPolygon &union_polygon);

// private:
    float dot(
        const float &x_1,
        const float &y_1,
        const float &x_2,
        const float &y_2);

    float dot(
        const EasyPoint2D &line_1_point_1,
        const EasyPoint2D &line_1_point_2,
        const EasyPoint2D &line_2_point_1,
        const EasyPoint2D &line_2_point_2);

    float cross(
        const float &x_1,
        const float &y_1,
        const float &x_2,
        const float &y_2);

    float cross(
        const EasyPoint2D &line_1_point_1,
        const EasyPoint2D &line_1_point_2,
        const EasyPoint2D &line_2_point_1,
        const EasyPoint2D &line_2_point_2);

    float lineLength2(
        const EasyPoint2D &line_point_1,
        const EasyPoint2D &line_point_2);

    float lineLength(
        const EasyPoint2D &line_point_1,
        const EasyPoint2D &line_point_2);

    float angle(
        const EasyPoint2D &line_1_point_1,
        const EasyPoint2D &line_1_point_2,
        const EasyPoint2D &line_2_point_1,
        const EasyPoint2D &line_2_point_2);

    bool isPointInRect(
        const float &point_x,
        const float &point_y,
        const float &rect_x_min,
        const float &rect_y_min,
        const float &rect_x_max,
        const float &rect_y_max);

    bool isPointInRect(
        const EasyPoint2D &point,
        const float &rect_x_min,
        const float &rect_y_min,
        const float &rect_x_max,
        const float &rect_y_max);

    bool isPointInLineRect(
        const EasyPoint2D &point,
        const EasyPoint2D &line_point_1,
        const EasyPoint2D &line_point_2);

    bool isRectCross(
        const EasyPoint2D &line_1_point_1,
        const EasyPoint2D &line_1_point_2,
        const EasyPoint2D &line_2_point_1,
        const EasyPoint2D &line_2_point_2);

    bool isLineCross(
        const EasyPoint2D &line_1_point_1,
        const EasyPoint2D &line_1_point_2,
        const EasyPoint2D &line_2_point_1,
        const EasyPoint2D &line_2_point_2);

    bool isLineParallel(
        const float &x_1,
        const float &y_1,
        const float &x_2,
        const float &y_2);

    bool isLineParallel(
        const EasyPoint2D &line_1_point_1,
        const EasyPoint2D &line_1_point_2,
        const EasyPoint2D &line_2_point_1,
        const EasyPoint2D &line_2_point_2);

    bool isPointOnOpenBoundedLine(
        const EasyPoint2D &point,
        const EasyPoint2D &line_point_1,
        const EasyPoint2D &line_point_2);

    bool isPointInPolygon(
        const EasyPoint2D &point);

    bool isPolygonCross(
        EasyPolygon &polygon);

    bool getLineCrossPoint(
        const EasyPoint2D &line_1_point_1,
        const EasyPoint2D &line_1_point_2,
        const EasyPoint2D &line_2_point_1,
        const EasyPoint2D &line_2_point_2,
        EasyPoint2D &line_cross_point);

    bool getBoundedLineCrossPoints(
        const EasyPoint2D &line_1_point_1,
        const EasyPoint2D &line_1_point_2,
        const EasyPoint2D &line_2_point_1,
        const EasyPoint2D &line_2_point_2,
        std::vector<EasyPoint2D> &line_cross_point_vec);

    bool isSamePoint(
        const EasyPoint2D &point_1,
        const EasyPoint2D &point_2);

    bool getPolygonIntersection(
        const EasyPolygon &polygon_1,
        const EasyPolygon &polygon_2,
        std::vector<EasyPoint2D> &intersect_point_vec);

private:
    std::vector<EasyPoint2D> point_list;
};

