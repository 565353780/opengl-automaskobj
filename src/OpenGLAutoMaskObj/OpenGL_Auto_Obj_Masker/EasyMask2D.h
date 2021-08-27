#ifndef EASYMASK2D_H
#define EASYMASK2D_H

#include <iostream>
#include <limits.h>

#include "EasyPoint2D.h"
#include "EasyRect2D.h"
#include "EasyLine2D.h"
#include "EasyIntersection2D.h"
#include "EasyPolygon2D.h"

class EasyMask2D
{
public:
    EasyMask2D()
    {
    }

    bool getUnionPolygonVec(
        EasyPolygon2D &polygon_1,
        EasyPolygon2D &polygon_2,
        std::vector<EasyPolygon2D> &union_polygon_vec);

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

    float pointDist2(
        const EasyPoint2D &point_1,
        const EasyPoint2D &point_2);

    float pointDist(
        const EasyPoint2D &point_1,
        const EasyPoint2D &point_2);

    float lineLength2(
        const EasyLine2D &line);

    float lineLength(
        const EasyLine2D &line);

    float angle(
        const EasyLine2D &line_1,
        const EasyLine2D &line_2);

    float clockWiseAngle(
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

    bool isPointInPolygon(
        const EasyPoint2D &point,
        const EasyPolygon2D &polygon);

    bool isPolygonCross(
        EasyPolygon2D &polygon_1,
        EasyPolygon2D &polygon_2);

    bool getLineCrossPoint(
        const EasyLine2D &line_1,
        const EasyLine2D &line_2,
        EasyPoint2D &line_cross_point);

    bool getBoundedLineCrossPointVec(
        const EasyLine2D &line_1,
        const EasyLine2D &line_2,
        std::vector<EasyPoint2D> &line_cross_point_vec);

    bool isSamePoint(
        const EasyPoint2D &point_1,
        const EasyPoint2D &point_2);

    bool getPolygonIntersection(
        EasyPolygon2D &polygon_1,
        EasyPolygon2D &polygon_2,
        std::vector<EasyIntersection2D> &intersection_vec);

    bool splitPolygonByIntersection(
        std::vector<EasyIntersection2D> &intersection_vec,
        EasyPolygon2D &polygon);

    bool haveThisPolygonIdx(
        const EasyIntersection2D &intersection_2d,
        const size_t &polygon_id,
        const size_t &line_idx);

    bool getIntersectionIdxVecOnPolygonLine(
        const std::vector<EasyIntersection2D> &intersection_vec,
        const size_t &polygon_id,
        const size_t &line_idx,
        std::vector<size_t> &intersection_idx_vec);

    bool updateAllPolygonLineConnectedState(
        const EasyPolygon2D &polygon,
        const EasyPolygon2D &union_polygon,
        std::vector<bool> &polygon_line_connected_vec);

    bool addNewPolygonPoint(
        const EasyPolygon2D &polygon_1,
        const EasyPolygon2D &polygon_2,
        const int &polygon_id,
        const int &start_line_idx,
        EasyPolygon2D &union_polygon,
        std::vector<bool> &polygon_1_line_connected_vec,
        std::vector<bool> &polygon_2_line_connected_vec);

    bool startLineIdxAdd1(
        const EasyPolygon2D &polygon_1,
        const EasyPolygon2D &polygon_2,
        const int &polygon_id,
        int &start_line_idx);

    bool getNearestIntersectionIdx(
        const EasyPolygon2D &polygon_1,
        const EasyPolygon2D &polygon_2,
        const std::vector<EasyIntersection2D> &intersection_vec,
        const int &polygon_id,
        const int &start_line_idx,
        const std::vector<size_t> &intersection_idx_vec,
        int &nearest_intersection_idx);

    bool getMinimalAnglePolygonPointIdx(
        const EasyPolygon2D &polygon_1,
        const EasyPolygon2D &polygon_2,
        const std::vector<EasyIntersection2D> &intersection_vec,
        const int &polygon_id,
        const int &start_line_idx,
        const int &start_intersection_idx,
        std::pair<int, int> &minimal_angle_polygon_point_idx);

    bool getUnionPolygonPoints(
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
        std::vector<bool> &polygon_2_line_connected_vec);

private:
    std::vector<EasyPolygon2D> polygon_list;
};

#endif //EASYMASK2D_H
