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
        const float &position_x_min,
        const float &position_y_min,
        const float &position_x_max,
        const float &position_y_max);

    float x_min;
    float y_min;
    float x_max;
    float y_max;
    float x_diff;
    float y_diff;
};

class EasyLine2D
{
public:
    EasyLine2D()
    {
    }

    bool setPosition(
        const float &position_x_1,
        const float &position_y_1,
        const float &position_x_2,
        const float &position_y_2);

    EasyPoint2D point_1;
    EasyPoint2D point_2;

    EasyRect2D rect;

    float x_diff;
    float y_diff;
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
        EasyRect2D &rect_2d);

    bool isPointInPolygon(
        const EasyPoint2D &point);

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
        const EasyLine2D &line_1,
        const EasyLine2D &line_2);

    float cross(
        const float &x_1,
        const float &y_1,
        const float &x_2,
        const float &y_2);

    float cross(
        const EasyLine2D &line_1,
        const EasyLine2D &line_2);

    float lineLength2(
        const EasyLine2D &line);

    float lineLength(
        const EasyLine2D &line);

    float angle(
        const EasyLine2D &line_1,
        const EasyLine2D &line_2);

    bool isPointInRect(
        const EasyPoint2D &point,
        const EasyRect2D &rect);

    bool isPointInLineRect(
        const EasyPoint2D &point,
        const EasyLine2D &line);

    bool isRectCross(
        const EasyLine2D &line_1,
        const EasyLine2D &line_2);

    bool isRectCross(
        const EasyRect2D &rect_1,
        const EasyRect2D &rect_2);

    bool isLineCross(
        const EasyLine2D &line_1,
        const EasyLine2D &line_2);

    bool isLineParallel(
        const EasyLine2D &line_1,
        const EasyLine2D &line_2);

    bool isPointOnOpenBoundedLine(
        const EasyPoint2D &point,
        const EasyLine2D &line);

    bool isPolygonCross(
        EasyPolygon &polygon);

    bool getLineCrossPoint(
        const EasyLine2D &line_1,
        const EasyLine2D &line_2,
        EasyPoint2D &line_cross_point);

    bool getBoundedLineCrossPoints(
        const EasyLine2D &line_1,
        const EasyLine2D &line_2,
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
    std::vector<EasyLine2D> line_list;
};

