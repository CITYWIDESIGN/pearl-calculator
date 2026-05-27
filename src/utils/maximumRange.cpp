//
// Created by cT on 2026/4/11.
//

#include "maximumRange.h"

#include <cmath>

#include "../data.h"
#include "../simulationPearl/calculatePearlInitialMotion.h"

bool isWithinMaximumRange(const Vector2 destination)
{
    // 使用最大 TNT 当量估算最远可达的初速度。
    const Vector3 initialMotion = calculateInitialPearlMotion(g_appState.maxTnt, g_appState.maxTnt, "North");

    double verticalPosition = 0.0;
    double verticalVelocity = initialMotion.y;
    int gameTick = 0;

    // 通过竖直方向模拟估算总飞行时间。
    if (g_appState.calculatorSettings.below1_21_1)
    {
        while (verticalPosition > -256.0 && gameTick < 10000)
        {
            verticalPosition += verticalVelocity;
            verticalVelocity *= 0.99;
            verticalVelocity -= 0.03;
            gameTick++;
        }
    }
    else
    {
        while (verticalPosition > -256.0 && gameTick < 10000)
        {
            verticalVelocity -= 0.03;
            verticalVelocity *= 0.99;
            verticalPosition += verticalVelocity;
            gameTick++;
        }
    }

    // 依据水平速度与空气阻力估算最大水平射程。
    const double horizontalSpeed = std::sqrt(initialMotion.x * initialMotion.x + initialMotion.z * initialMotion.z);
    const double maximumHorizontalRange = horizontalSpeed * 100.0 * (1 - std::pow(0.99, gameTick));
    const double targetDistance = std::sqrt(destination.x * destination.x + destination.z * destination.z);

    return targetDistance <= maximumHorizontalRange;
}