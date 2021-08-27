#include "EasyRect2D.h"

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


