#include "EasyLine2D.h"

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

bool EasyLine2D::setPosition(
    const EasyPoint2D &point_1_data,
    const EasyPoint2D &point_2_data)
{
    return setPosition(
        point_1_data.x,
        point_1_data.y,
        point_2_data.x,
        point_2_data.y);
}

