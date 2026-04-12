//
// Created by cT on 2026/4/11.
//
#include <algorithm>

#include "solve.h"
#include "calculateTNT.h"
#include "../data.h"
#include "../utils/maximumRange.h"
#include "../simulationPearl/simulatePearl.h"

#define MAX_TRAVEL_TICK 1000

// 实际落点距离理论落点的偏移距离
static double offset(vector2 destination, const Configuration& calculateResult, int consumingTicks)
{
    // 模拟珍珠轨迹
    const std::vector<Simulate> results = simulatePearl(calculateResult, consumingTicks);

    // 检查结果是否有效
    if (results.size() <= consumingTicks)
    {
        return -1.0;
    }

    // 获取实际珍珠在指定 tick 的水平位置
    const vector2 realPosition = {
        results[consumingTicks].Position.X,
        results[consumingTicks].Position.Z
    };

    // 计算两点之间的距离
    const double dx = realPosition.X - destination.X;
    const double dz = realPosition.Z - destination.Z;

    return std::sqrt(dx * dx + dz * dz);
}

// 求解函数
std::vector<Plan> solve(vector2 destination)
{
    // 排除超出射程的落点
    if (!maximumRange(destination))
    {
        return {};
    }

    // 定义动态结构体
    std::vector<Plan> candidates;
    // 预留内存空间
    candidates.reserve(1331);
    int additional = 0;

    for (int gameTick = 1; gameTick < MAX_TRAVEL_TICK; gameTick++)
    {
        // 理论计算结果
        const Configuration baseConfig = calculateTNT(destination, gameTick);

        // 超出上限跳过本次循环
        if (baseConfig.redTNT > maxTNT || baseConfig.blueTNT > maxTNT)
        {
            continue;
        }

        // 执行 Y 坐标判断逻辑
        if (calculatorConfig.accuracyPriority)
        {

        }

        // 局部搜索
        for (int r = std::max(0, baseConfig.redTNT - 5); r <= baseConfig.redTNT + 5; ++r)
        {
            for (int b = std::max(0, baseConfig.blueTNT - 5); b <= baseConfig.blueTNT + 5; ++b)
            {
                Configuration currentTry = {r, b, baseConfig.direction};
                const double currentOffset = offset(destination, currentTry, gameTick);

                // 偏移量在可接受范围才加入 过滤极端错误解
                if (currentOffset >= 0 && currentOffset < 10.0)
                {
                    candidates.push_back({currentOffset, currentTry, gameTick});
                }
            }
        }

        // 多重采样
        if (!candidates.empty())
        {
            if (additional >= 10) break;
            additional++;
        }
    }

    // 排序算法
    if (calculatorConfig.accuracyPriority)
    {
        // 优先准度 (landingOffset) -> 其次速度 (arriveGameTick)
        std::ranges::sort(candidates, [](const Plan& a, const Plan& b) {
            // 防止浮点数精度抖动
            if (std::abs(a.landingOffset - b.landingOffset) > 1e-9) {
                return a.landingOffset < b.landingOffset;
            }
            // 准度相同时，耗时短的优先
            return a.arriveGameTick < b.arriveGameTick;
        });
    }
    else
    {
        // 优先速度 (arriveGameTick) -> 其次准度 (landingOffset)
        std::ranges::sort(candidates, [](const Plan& a, const Plan& b) {
            if (a.arriveGameTick != b.arriveGameTick) {
                return a.arriveGameTick < b.arriveGameTick;
            }
            // 速度相同时，准度高的优先
            return a.landingOffset < b.landingOffset;
        });
    }

    // 返回候选结果
    return candidates;
}
