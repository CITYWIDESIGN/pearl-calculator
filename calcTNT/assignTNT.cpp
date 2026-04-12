//
// Created by cT on 2026/4/11.
//


#include "assignTNT.h"

#include "../data.h"

enum class Direction
{
    North,
    South,
    East,
    West
};

// 用来描述方向方案
struct Candidate
{
    // 方向
    Direction dir;
    // 该方向的总推力
    double weight;
    // 两组TNT的数量
    double valA, valB;

    // 角A与角B的名字
    const char* nameA;
    const char* nameB;
};

Configuration assignTNT(const Layout& layout)
{
    // 构造四个方向的候选方案
    const Candidate candidates[] = {
        // 北方向
        {
            Direction::North,
            layout.SouthEast + layout.SouthWest,
            layout.SouthEast, layout.SouthWest,
            "SouthEast", "SouthWest"
        },

        // 南方向
        {
            Direction::South,
            layout.NorthEast + layout.NorthWest,
            layout.NorthEast, layout.NorthWest,
            "NorthEast", "NorthWest"
        },

        // 西方向
        {
            Direction::West,
            layout.NorthEast + layout.SouthEast,
            layout.NorthEast, layout.SouthEast,
            "NorthEast", "SouthEast"
        },

        // 东方向
        {
            Direction::East,
            layout.NorthWest + layout.SouthWest,
            layout.NorthWest, layout.SouthWest,
            "NorthWest", "SouthWest"
        }
    };

    // 找出推力最大的方向
    const Candidate* best = &candidates[0];

    for (const auto& c : candidates)
    {
        if (c.weight > best->weight)
        {
            best = &c;
        }
    }

    // 据默认方向分配红/蓝 TNT
    int redTNT = 0, blueTNT = 0;

    // 情况1：A角是默认红色
    if (best->nameA == defaultRedTNTDirection)
    {
        redTNT = std::round(best->valA);
        blueTNT = std::round(best->valB);
    }

    // 情况2：B角是默认红色
    else if (best->nameB == defaultRedTNTDirection)

    {
        redTNT = std::round(best->valB);
        blueTNT = std::round(best->valA);
    }

    // 情况3：A角是默认蓝色
    else if (best->nameA == defaultBlueTNTDirection)

    {
        blueTNT = std::round(best->valA);
        redTNT = std::round(best->valB);
    }

    // 情况4：B角是默认蓝色
    else

    {
        blueTNT = std::round(best->valB);
        redTNT = std::round(best->valA);
    }

    // 转回字符串
    std::string direction;

    switch (best->dir)
    {
    case Direction::North: direction = "North";
        break;
    case Direction::South: direction = "South";
        break;
    case Direction::East: direction = "East";
        break;
    case Direction::West: direction = "West";
        break;
    }

    // 返回结果
    return {redTNT, blueTNT, direction};
}
