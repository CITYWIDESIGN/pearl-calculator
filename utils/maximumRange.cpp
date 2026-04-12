//
// Created by cT on 2026/4/11.
//

#include "maximumRange.h"
#include "../data.h"
#include "../simulationPearl/calculatePearlInitialMotion.h"

// 最大射程
bool maximumRange(vector2 destination)
{
    // 最大当量下的初始 Motion
    const vector3 initialMotion = calculatePearlInitialMotion(maxTNT, maxTNT, "North");

    // 定义最大射程
    double maximumRange = 0;

    double y = 0.0;
    double v = initialMotion.Y;

    // 计算最大飞行时间
    int tick = 0;

    if (calculatorConfig.below1_21_1)
    {
        while (y > -256.0 && tick < 10000)
        {
            y += v;
            v *= 0.99;
            v -= 0.03;
            tick++;
        }
    }
    else
    {
        while (y > -256.0 && tick < 10000)
        {
            v -= 0.03;
            v *= 0.99;
            y += v;
            tick++;
        }
    }

    // 计算最大射程
    const double horizontalSpeed = std::sqrt(initialMotion.X * initialMotion.X + initialMotion.Z * initialMotion.Z);

    maximumRange = horizontalSpeed * 100.0 * (1 - pow(0.99, tick));

    // 判断是否在最大射程内
    const double targetDistance = std::sqrt(
        destination.X * destination.X +
        destination.Z * destination.Z
    );

    return targetDistance <= maximumRange;
}
