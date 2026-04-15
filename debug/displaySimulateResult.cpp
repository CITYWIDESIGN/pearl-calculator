//
// Created by cT on 2026/4/11.
//

#include "displaySimulateResult.h"

#include <iomanip>
#include <iostream>
#include <vector>

void displaySimulationResult(const std::vector<SimulationPoint>& trajectory)
{
    std::cout << "珍珠模拟结果" << std::endl << std::endl;

    // 打印统一精度的表头与轨迹数据。
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "Tick\tPositionX\tPositionY\tPositionZ\tMotionX    \tMotionY   \tMotionZ" << std::endl << std::endl;

    for (const auto& [position, motion, gameTick] : trajectory)
    {
        std::cout << gameTick << "  \t"
            << position.x << "\t" << position.y << "\t" << position.z << "\t"
            << motion.x << "\t" << motion.y << "\t" << motion.z << std::endl;
    }

    std::cout << "\n总计模拟时长: " << trajectory.size() - 1 << " ticks" << std::endl;
}