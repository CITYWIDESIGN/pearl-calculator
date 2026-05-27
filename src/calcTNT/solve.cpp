//
// Created by cT on 2026/4/11.
//

#include "solve.h"

#include <algorithm>
#include <cmath>
#include <vector>

#include "../data.h"
#include "../simulationPearl/simulatePearl.h"
#include "../utils/maximumRange.h"
#include "calculateTNT.h"

namespace
{
    constexpr int kMaxTravelTick = 1000;

    double calculateLandingOffset(
        const Vector2 destination,
        const TntConfiguration& launchConfiguration,
        const int travelTicks
    )
    {
        // 通过轨迹模拟获取指定刻的真实落点。
        const std::vector<SimulationPoint> trajectory = simulatePearlTrajectory(launchConfiguration, travelTicks);
        if (trajectory.size() <= static_cast<std::size_t>(travelTicks))
        {
            return -1.0;
        }

        const Vector2 actualPosition = {
            trajectory[travelTicks].position.x,
            trajectory[travelTicks].position.z
        };
        const double deltaX = actualPosition.x - destination.x;
        const double deltaZ = actualPosition.z - destination.z;

        return std::sqrt(deltaX * deltaX + deltaZ * deltaZ);
    }
}

std::vector<LaunchPlan> solveLaunchPlans(const Vector2 destination)
{
    // 先排除明显超出射程的目标点。
    if (!isWithinMaximumRange(destination))
    {
        return {};
    }

    std::vector<LaunchPlan> candidates;
    candidates.reserve(1331);

    int additionalSamples = 0;

    for (int gameTick = 1; gameTick < kMaxTravelTick; gameTick++)
    {
        // 先计算理论 TNT 配置。
        const TntConfiguration baseConfiguration = calculateTntRequirement(destination, gameTick);

        // 超出最大 TNT 上限时跳过。
        if (baseConfiguration.redTnt > g_appState.maxTnt || baseConfiguration.blueTnt > g_appState.maxTnt)
        {
            continue;
        }

        // 在指定 Y 模式下，要求轨迹刚好穿过目标平面。
        if (g_appState.calculatorSettings.specifiesYPosition)
        {
            const std::vector<SimulationPoint> trajectory = simulatePearlTrajectory(baseConfiguration, gameTick + 1);
            const bool crossesTargetPlane =
                trajectory[gameTick].position.y > g_appState.targetYPosition &&
                trajectory[gameTick + 1].position.y <= g_appState.targetYPosition;

            if (!crossesTargetPlane)
            {
                continue;
            }
        }

        // 在理论解附近做局部搜索，补偿离散 TNT 数量误差。
        for (int redTnt = std::max(0, baseConfiguration.redTnt - 5); redTnt <= baseConfiguration.redTnt + 5; ++redTnt)
        {
            for (int blueTnt = std::max(0, baseConfiguration.blueTnt - 5); blueTnt <= baseConfiguration.blueTnt + 5; ++
                 blueTnt)
            {
                const TntConfiguration currentConfiguration = {redTnt, blueTnt, baseConfiguration.direction};
                const double currentOffset = calculateLandingOffset(destination, currentConfiguration, gameTick);

                bool isValidCandidate = false;

                // 指定 Y 模式下沿用原本更宽松的水平误差判断。
                if (g_appState.calculatorSettings.specifiesYPosition)
                {
                    isValidCandidate = currentOffset >= 0.0 && currentOffset < 150.0;
                }
                else
                {
                    isValidCandidate = currentOffset >= 0.0 && currentOffset < 10.0;
                }

                if (isValidCandidate)
                {
                    candidates.push_back({currentOffset, currentConfiguration, gameTick});
                }
            }
        }

        // 保留原有的多重采样提前停止策略。
        if (!candidates.empty())
        {
            if (additionalSamples >= 100)
            {
                break;
            }

            additionalSamples++;
        }
    }

    // 按配置要求决定排序优先级。
    if (g_appState.calculatorSettings.accuracyPriority)
    {
        std::ranges::sort(candidates, [](const LaunchPlan& left, const LaunchPlan& right)
        {
            if (std::abs(left.landingOffset - right.landingOffset) > 1e-9)
            {
                return left.landingOffset < right.landingOffset;
            }

            return left.arrivalGameTick < right.arrivalGameTick;
        });
    }
    else
    {
        std::ranges::sort(candidates, [](const LaunchPlan& left, const LaunchPlan& right)
        {
            if (left.arrivalGameTick != right.arrivalGameTick)
            {
                return left.arrivalGameTick < right.arrivalGameTick;
            }

            return left.landingOffset < right.landingOffset;
        });
    }

    return candidates;
}
