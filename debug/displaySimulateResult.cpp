//
// Created by cT on 2026/4/11.
//
#include <iostream>
#include <vector>
#include <iomanip>

#include "displaySimulateResult.h"
#include "../data.h"

void displaySimulateResult(const std::vector<Simulate>& trajectory)
{
    std::cout << "珍珠模拟结果" << std::endl << std::endl;

    // 打印表头
    // 保留6位小数
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "Tick\tPositionX\tPositionY\tPositionZ\tMotionX    \tMotionY   \tMotionZ" << std::endl << std::endl;

    // 遍历 vector
    for (const auto& [Position, Motion, GameTick] : trajectory)
    {
        std::cout << GameTick << "  \t"
        << Position.X << "\t" << Position.Y << "\t" << Position.Z << "\t"
        << Motion.X << "\t" << Motion.Y << "\t" << Motion.Z << std::endl;
    }
    std::cout << "\n总计模拟时长: " << trajectory.size() - 1 << " ticks" << std::endl;
}
