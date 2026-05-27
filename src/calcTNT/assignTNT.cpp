//
// Created by cT on 2026/4/11.
//

#include "assignTNT.h"

#include <cmath>

#include "../data.h"

namespace
{
    enum class LaunchDirection
    {
        North,
        South,
        East,
        West
    };

    struct DirectionCandidate
    {
        LaunchDirection direction;
        double weight;
        double primaryTntCount;
        double secondaryTntCount;
        const char* primarySlotName;
        const char* secondarySlotName;
    };
}

TntConfiguration assignTntConfiguration(const BarrelLayout& layout)
{
    // 构造四个方向的候选方案。
    const DirectionCandidate candidates[] = {
        {
            LaunchDirection::North,
            layout.southEast + layout.southWest,
            layout.southEast,
            layout.southWest,
            "SouthEast",
            "SouthWest"
        },
        {
            LaunchDirection::South,
            layout.northEast + layout.northWest,
            layout.northEast,
            layout.northWest,
            "NorthEast",
            "NorthWest"
        },
        {
            LaunchDirection::West,
            layout.northEast + layout.southEast,
            layout.northEast,
            layout.southEast,
            "NorthEast",
            "SouthEast"
        },
        {
            LaunchDirection::East,
            layout.northWest + layout.southWest,
            layout.northWest,
            layout.southWest,
            "NorthWest",
            "SouthWest"
        }
    };

    // 选出总推力最大的方向。
    const DirectionCandidate* bestCandidate = &candidates[0];
    for (const auto& candidate : candidates)
    {
        if (candidate.weight > bestCandidate->weight)
        {
            bestCandidate = &candidate;
        }
    }

    int redTntCount = 0;
    int blueTntCount = 0;

    // 按默认红蓝 TNT 朝向映射最终数量。
    if (bestCandidate->primarySlotName == g_appState.defaultRedTntDirection)
    {
        redTntCount = std::round(bestCandidate->primaryTntCount);
        blueTntCount = std::round(bestCandidate->secondaryTntCount);
    }
    else if (bestCandidate->secondarySlotName == g_appState.defaultRedTntDirection)
    {
        redTntCount = std::round(bestCandidate->secondaryTntCount);
        blueTntCount = std::round(bestCandidate->primaryTntCount);
    }
    else if (bestCandidate->primarySlotName == g_appState.defaultBlueTntDirection)
    {
        blueTntCount = std::round(bestCandidate->primaryTntCount);
        redTntCount = std::round(bestCandidate->secondaryTntCount);
    }
    else
    {
        blueTntCount = std::round(bestCandidate->secondaryTntCount);
        redTntCount = std::round(bestCandidate->primaryTntCount);
    }

    std::string direction;

    // 将枚举方向转换回配置字符串。
    switch (bestCandidate->direction)
    {
    case LaunchDirection::North:
        direction = "North";
        break;
    case LaunchDirection::South:
        direction = "South";
        break;
    case LaunchDirection::East:
        direction = "East";
        break;
    case LaunchDirection::West:
        direction = "West";
        break;
    }

    return {redTntCount, blueTntCount, direction};
}