#ifndef EASYPOLYGON_H
#define EASYPOLYGON_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <stdlib.h>
#include <cmath>

#include "EasyPoint2D.h"
#include "EasyRect2D.h"
#include "EasyLine2D.h"
#include "EasyIntersection2D.h"

class EasyPolygon2D
{
public:
    EasyPolygon2D()
    {
    }

    bool reset();

    bool setID(
        const int &id_data);

    bool addPoint(
        const EasyPoint2D &point_2d);

    bool insertPoint(
        const EasyPoint2D &point_2d,
        const size_t &insert_idx1);

    bool removePoint(
        const size_t &remove_idx);

    bool update();

    bool updateLineVec();

    bool isClockWise();

    bool setClockWise();

    bool setAntiClockWise();

    float getPolygonArea();

    bool getPolygonRect(
        EasyRect2D &rect_2d);

    int id;

    std::vector<EasyPoint2D> point_list;
    std::vector<EasyLine2D> line_list;
};

#endif //EASYPOLYGON_H
