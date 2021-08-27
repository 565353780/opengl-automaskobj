#include "EasyPoint2D.h"

bool EasyPoint2D::setPosition(
    const float &position_x,
    const float &position_y)
{
    x = position_x;
    y = position_y;

    return true;
}

