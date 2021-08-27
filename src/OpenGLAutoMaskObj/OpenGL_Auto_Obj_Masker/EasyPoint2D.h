#ifndef EASYPOINT2D_H
#define EASYPOINT2D_H

#include <iostream>

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

#endif //EASYPOINT2D_H
