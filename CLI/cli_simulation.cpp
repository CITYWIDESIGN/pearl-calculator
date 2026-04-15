//
// Created by cT on 2026/4/15.
//

#include "cli_simulation.h"

#include <iostream>
#include <string>
#include <vector>

#include "../debug/displaySimulateResult.h"
#include "../simulationPearl/simulatePearl.h"
#include "../utils/clearScreen.h"
#include "cli_common.h"

namespace
{
    bool isValidDirection(const std::string& direction)
    {
        return direction == "East" ||
            direction == "South" ||
            direction == "West" ||
            direction == "North";
    }

    bool readManualSimulationConfiguration(TntConfiguration& configuration, int& simulationTicks)
    {
        std::cout << "请输入 Red TNT 数量: ";
        if (!(std::cin >> configuration.redTnt))
        {
            clearCliInputBuffer();
            return false;
        }

        std::cout << "请输入 Blue TNT 数量: ";
        if (!(std::cin >> configuration.blueTnt))
        {
            clearCliInputBuffer();
            return false;
        }

        while (true)
        {
            std::cout << "请输入方向 (East, South, West, North): ";
            if (!(std::cin >> configuration.direction))
            {
                clearCliInputBuffer();
                return false;
            }

            if (isValidDirection(configuration.direction))
            {
                break;
            }

            std::cout << "[错误] 无效方向，请重新输入！" << std::endl;
        }

        std::cout << "请输入模拟时长: ";
        if (!(std::cin >> simulationTicks))
        {
            clearCliInputBuffer();
            return false;
        }

        clearCliInputBuffer();
        return true;
    }
}

void runCliPearlSimulation(const bool isManualMode, const LaunchPlan& selectedPlan)
{
    TntConfiguration simulationConfiguration;
    int simulationTicks = 0;

    if (isManualMode)
    {
        if (!readManualSimulationConfiguration(simulationConfiguration, simulationTicks))
        {
            std::cout << "输入无效，已返回菜单。" << std::endl;
            waitForCliEnter();
            return;
        }

        clearScreen();
    }
    else
    {
        simulationConfiguration = selectedPlan.configuration;
        simulationTicks = selectedPlan.arrivalGameTick + 10;
        clearScreen();
    }

    std::cout << "[珍珠模拟]\n\nRed\t" << simulationConfiguration.redTnt
        << "\nBlue\t" << simulationConfiguration.blueTnt
        << "\nDir\t" << simulationConfiguration.direction
        << std::endl << std::endl;

    const std::vector<SimulationPoint> trajectory =
        simulatePearlTrajectory(simulationConfiguration, simulationTicks);
    displaySimulationResult(trajectory);

    if (!isManualMode)
    {
        for (const auto& point : trajectory)
        {
            if (point.gameTick == selectedPlan.arrivalGameTick)
            {
                std::cout << "\n>>> 珍珠于 " << point.gameTick << " GT 到达目标坐标。" << std::endl;
                break;
            }
        }
    }

    waitForCliEnter();
}