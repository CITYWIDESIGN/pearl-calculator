//
// Created by cT on 2026/4/10.
//

#include <cmath>

#include "getDistance3D.h"

double calculateDistance3D(const Vector3& startPosition, const Vector3& endPosition)
{
    const double deltaX = endPosition.x - startPosition.x;
    const double deltaY = endPosition.y - startPosition.y;
    const double deltaZ = endPosition.z - startPosition.z;

    return std::sqrt(deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ);
}