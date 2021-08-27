#ifndef INTERSECTION2DMANAGER_H
#define INTERSECTION2DMANAGER_H

#include <iostream>

#include "EasyIntersection2D.h"

class Intersection2DManager
{
public:
  Intersection2DManager()
  {
  }

private:
  std::vector<EasyIntersection2D> intersection_vec_;
};

#endif //INTERSECTION2DMANAGER_H
