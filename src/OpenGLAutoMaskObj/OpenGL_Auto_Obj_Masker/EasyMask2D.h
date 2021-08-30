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
        std::vector<EasyPolygon2D> &polygon_vec,
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

    float getClockWiseAngle(
        const EasyLine2D &line_1,
        const EasyLine2D &line_2);

    float getAntiClockWiseAngle(
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
        std::vector<EasyPolygon2D> &polygon_vec,
        std::vector<EasyIntersection2D> &intersection_vec);

    bool getSortedIntersectionOnPolygonLine(
        const EasyPolygon2D &polygon,
        const std::vector<EasyIntersection2D> &intersection_vec,
        const size_t &polygon_idx,
        const size_t &point_idx,
        std::vector<size_t> &sorted_intersection_idx_on_polygon_line_vec);

    bool getSplitPolygonAndIntersectionPosition(
        const EasyPolygon2D &polygon,
        const std::vector<EasyIntersection2D> &intersection_vec,
        const size_t &polygon_idx,
        EasyPolygon2D &split_polygon,
        std::vector<std::pair<size_t, size_t>> &intersection_idx_polygon_point_idx_pair_vec);

    bool splitPolygonsByIntersection(
        const std::vector<EasyIntersection2D> &intersection_vec,
        const std::vector<EasyPolygon2D> &polygon_vec,
        std::vector<EasyIntersection2D> &split_intersection_vec,
        std::vector<EasyPolygon2D> &split_polygon_vec);

    bool getIntersectionIdxOnPolygonPoint(
        const std::vector<EasyPolygon2D> &polygon_vec,
        const std::vector<EasyIntersection2D> &intersection_vec,
        const size_t &polygon_idx,
        const size_t &point_idx,
        size_t &intersection_idx);

    bool updatePolygonIntersectionPointConnectedState(
        const EasyIntersection2D &intersection,
        std::vector<std::vector<bool>> &polygon_point_connected_vec_vec);

    bool updatePolygonNotIntersectionPointConnectedState(
        const std::vector<EasyPolygon2D> &polygon_vec,
        const EasyPolygon2D &union_polygon,
        std::vector<std::vector<bool>> &polygon_point_connected_vec_vec);

    bool addNewPolygonPoint(
        const std::vector<EasyPolygon2D> &polygon_vec,
        const size_t &polygon_idx,
        const size_t &start_point_idx,
        EasyPolygon2D &union_polygon,
        std::vector<std::vector<bool>> &polygon_point_connected_vec_vec);

    bool getMinimalAnglePolygonPointIdx(
        const std::vector<EasyPolygon2D> &polygon_vec,
        const std::vector<EasyIntersection2D> &intersection_vec,
        const size_t &polygon_idx,
        const size_t &start_point_idx,
        const size_t &start_intersection_idx,
        std::pair<size_t, size_t> &minimal_angle_polygon_point_idx);

    bool getUnionPolygonPoints(
        const std::vector<EasyPolygon2D> &polygon_vec,
        const std::vector<EasyIntersection2D> &intersection_vec,
        const size_t &start_polygon_idx,
        const size_t &start_point_idx,
        const size_t &current_start_polygon_idx,
        const size_t &current_start_point_idx,
        EasyPolygon2D &union_polygon,
        std::vector<std::vector<bool>> &polygon_point_connected_vec_vec);

private:
    size_t connected_polygon_point_num_=0;
};

#endif //EASYMASK2D_H
