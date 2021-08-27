#ifndef EASYRECT2D_H
#define EASYRECT2D_H

#include <iostream>

#include "EasyPoint2D.h"

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

#endif //EASYRECT2D_H
