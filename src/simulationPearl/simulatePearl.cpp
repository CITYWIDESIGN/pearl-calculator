//
// Created by cT on 2026/4/11.
//

#include "simulatePearl.h"

#include <vector>

#include "../data.h"
#include "calculatePearlInitialMotion.h"

std::vector<SimulationPoint> simulatePearlTrajectory(
    const TntConfiguration& simulateConfiguration,
    const int simulateTick
)
{
    Vector3 position = g_appState.pearl.position;
    Vector3 motion = calculateInitialPearlMotion(
        simulateConfiguration.redTnt,
        simulateConfiguration.blueTnt,
        simulateConfiguration.direction
    );

    std::vector<SimulationPoint> trajectory;
    trajectory.reserve(simulateTick + 1);

    // 根据版本差异执行对应的每 tick 物理顺序。
    if (g_appState.calculatorSettings.below1_21_1)
    {
        for (int gameTick = 0; gameTick <= simulateTick; gameTick++)
        {
            trajectory.push_back({position, motion, gameTick});
            position = position + motion;
            motion = motion * 0.99;
            motion.y -= 0.03;
        }
    }
    else
    {
        for (int gameTick = 0; gameTick <= simulateTick; gameTick++)
        {
            trajectory.push_back({position, motion, gameTick});
            motion.y -= 0.03;
            motion = motion * 0.99;
            position = position + motion;
        }
    }

    return trajectory;
}
