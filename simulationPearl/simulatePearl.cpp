//
// Created by cT on 2026/4/11.
//
#include <vector>

#include "simulatePearl.h"
#include "calculatePearlInitialMotion.h"
#include "../data.h"

std::vector<Simulate> simulatePearl(const Configuration& simulateConfig, const int simulateTick)
{
    // 珍珠第 0gt 的坐标
    vector3 Position = pearl.Position;

    // 计算 珍珠第 0gt 的 Motion
    vector3 Motion =
        calculatePearlInitialMotion(simulateConfig.redTNT, simulateConfig.blueTNT,simulateConfig.direction);

    // 定义结构体
    std::vector<Simulate> trajectory;
    trajectory.reserve(simulateTick + 1);

    // 执行版本判断逻辑
    if (calculatorConfig.below1_21_1)
    {
        // 1.21.1 and below
        // 从 0gt 模拟到指定的 simulateTick
        for (int gameTick = 0; gameTick <= simulateTick; gameTick++)
        {
            // 写入当前 gameTick 珍珠的状态
            trajectory.push_back({Position, Motion, gameTick});

            // 珍珠先移动
            Position = Position + Motion;
            // 空气阻力 重力
            Motion = Motion * 0.99;
            Motion.Y -= 0.03;
        }
    }
    else
    {
        // 1.21.1 and above
        // 从 0gt 模拟到指定的 simulateTick
        for (int gameTick = 0; gameTick <= simulateTick; gameTick++)
        {
            // 写入当前 gameTick 珍珠的状态
            trajectory.push_back({Position, Motion, gameTick});

            // 重力 空气阻力
            Motion.Y -= 0.03;
            Motion = Motion * 0.99;
            // 珍珠再移动
            Position = Position + Motion;
        }
    }

    return trajectory;
}