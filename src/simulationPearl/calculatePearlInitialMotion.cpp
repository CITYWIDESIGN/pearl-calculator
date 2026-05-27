//
// Created by cT on 2026/4/11.
//

#include "calculatePearlInitialMotion.h"

#include <string>

#include "../data.h"
#include "../utils/calculateSingleTNTContribution.h"

namespace
{
void distributeTntPositions(const std::string& direction, Vector3& redTntPosition, Vector3& blueTntPosition)
{
    Vector3 primaryPosition{};
    Vector3 secondaryPosition{};
    std::string primaryPositionName;
    std::string secondaryPositionName;

    // 根据发射方向选择两侧 TNT 槽位。
    if (direction == "North")
    {
        primaryPosition = g_appState.tntPositions.southEast;
        secondaryPosition = g_appState.tntPositions.southWest;
        primaryPositionName = "SouthEast";
        secondaryPositionName = "SouthWest";
    }
    else if (direction == "South")
    {
        primaryPosition = g_appState.tntPositions.northEast;
        secondaryPosition = g_appState.tntPositions.northWest;
        primaryPositionName = "NorthEast";
        secondaryPositionName = "NorthWest";
    }
    else if (direction == "West")
    {
        primaryPosition = g_appState.tntPositions.northEast;
        secondaryPosition = g_appState.tntPositions.southEast;
        primaryPositionName = "NorthEast";
        secondaryPositionName = "SouthEast";
    }
    else if (direction == "East")
    {
        primaryPosition = g_appState.tntPositions.northWest;
        secondaryPosition = g_appState.tntPositions.southWest;
        primaryPositionName = "NorthWest";
        secondaryPositionName = "SouthWest";
    }
    else
    {
        return;
    }

    // 按默认红蓝槽位方向分配具体 TNT 位置。
    if (primaryPositionName == g_appState.defaultRedTntDirection)
    {
        redTntPosition = primaryPosition;
        blueTntPosition = secondaryPosition;
    }
    else if (secondaryPositionName == g_appState.defaultRedTntDirection)
    {
        redTntPosition = secondaryPosition;
        blueTntPosition = primaryPosition;
    }
    else if (primaryPositionName == g_appState.defaultBlueTntDirection)
    {
        redTntPosition = secondaryPosition;
        blueTntPosition = primaryPosition;
    }
    else if (secondaryPositionName == g_appState.defaultBlueTntDirection)
    {
        redTntPosition = primaryPosition;
        blueTntPosition = secondaryPosition;
    }
}
}

Vector3 calculateInitialPearlMotion(
    const int redTntCount,
    const int blueTntCount,
    const std::string& direction
)
{
    // 没有 TNT 助推时，直接返回原始速度。
    if (redTntCount == 0 && blueTntCount == 0)
    {
        return g_appState.pearl.motion;
    }

    Vector3 redTntPosition{};
    Vector3 blueTntPosition{};

    // 根据方向计算红蓝 TNT 的实际坐标。
    distributeTntPositions(direction, redTntPosition, blueTntPosition);

    // 计算红蓝 TNT 对珍珠的初始推力。
    const Vector3 redTntMotion =
        calculateSingleTntContribution(redTntPosition, g_appState.pearl.position) * redTntCount;
    const Vector3 blueTntMotion =
        calculateSingleTntContribution(blueTntPosition, g_appState.pearl.position) * blueTntCount;

    return redTntMotion + blueTntMotion + g_appState.pearl.motion;
}
