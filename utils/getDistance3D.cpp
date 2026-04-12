//
// Created by cT on 2026/4/10.
//
#include <cmath>

#include "getDistance3D.h"
#include "../data.h"

double getDistance3D(const vector3& posA, const vector3& posB)
{
    const double dx = posB.X - posA.X;
    const double dy = posB.Y - posA.Y;
    const double dz = posB.Z - posA.Z;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}