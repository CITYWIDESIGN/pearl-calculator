//
// Created by cT on 2026/4/15.
//

#include "cli_calculation.h"

#include <conio.h>
#include <iostream>
#include <vector>

#include "../calcTNT/solve.h"
#include "../data.h"
#include "../debug/displaySolve.h"
#include "../utils/clearScreen.h"
#include "cli_common.h"
#include "cli_simulation.h"

namespace
{
    bool readDestinationMode(int& selectedMode)
    {
        std::cout << "[模式选择]\n" << std::endl;
        std::cout << "1. 默认\n2. 指定落点 Y 坐标\n\n请选择模式：";

        if (!(std::cin >> selectedMode))
        {
            clearCliInputBuffer();
            return false;
        }

        clearScreen();
        return true;
    }

    bool configureDefaultMode()
    {
        double destinationX = 0.0;
        double destinationZ = 0.0;

        g_appState.calculatorSettings.accuracyPriority = false;
        g_appState.calculatorSettings.specifiesYPosition = false;

        std::cout << "[默认模式]\n" << std::endl;
        std::cout << "目的地 X: ";
        if (!(std::cin >> destinationX))
        {
            clearCliInputBuffer();
            return false;
        }

        std::cout << "目的地 Z: ";
        if (!(std::cin >> destinationZ))
        {
            clearCliInputBuffer();
            return false;
        }

        g_appState.destination = {destinationX, 0.0, destinationZ};
        g_appState.targetYPosition = 0.0;
        return true;
    }

    bool configureYSpecifiedMode()
    {
        double destinationX = 0.0;
        double destinationY = 0.0;
        double destinationZ = 0.0;

        g_appState.calculatorSettings.accuracyPriority = true;
        g_appState.calculatorSettings.specifiesYPosition = true;

        std::cout << "[指定 Y 坐标模式]\n" << std::endl;
        std::cout << "目的地 X: ";
        if (!(std::cin >> destinationX))
        {
            clearCliInputBuffer();
            return false;
        }

        std::cout << "目的地 Y: ";
        if (!(std::cin >> destinationY))
        {
            clearCliInputBuffer();
            return false;
        }

        std::cout << "目的地 Z: ";
        if (!(std::cin >> destinationZ))
        {
            clearCliInputBuffer();
            return false;
        }

        g_appState.destination = {destinationX, destinationY, destinationZ};
        g_appState.targetYPosition = destinationY;
        return true;
    }

    bool configureCalculationTarget()
    {
        int selectedMode = 0;
        if (!readDestinationMode(selectedMode))
        {
            std::cout << "输入无效，已返回菜单。" << std::endl;
            waitForCliEnter();
            return false;
        }

        if (selectedMode == 1)
        {
            return configureDefaultMode();
        }

        if (selectedMode == 2)
        {
            return configureYSpecifiedMode();
        }

        std::cout << "暂不支持该模式。" << std::endl;
        waitForCliEnter();
        return false;
    }

    void promptSimulationFromPlans(const std::vector<LaunchPlan>& plans)
    {
        std::cout << "按 [S] 开启轨迹模拟，或按其他任意键返回..." << std::endl;
        const char pressedKey = static_cast<char>(_getch());

        if (pressedKey != 's' && pressedKey != 'S')
        {
            return;
        }

        std::cout << "请选择要模拟的方案编号 (1-" << plans.size() << "): ";

        int selectedPlanIndex = 0;
        if (!(std::cin >> selectedPlanIndex))
        {
            clearCliInputBuffer();
            std::cout << "输入无效，已返回菜单。" << std::endl;
            waitForCliEnter();
            return;
        }

        const int zeroBasedIndex = selectedPlanIndex - 1;
        if (zeroBasedIndex < 0 || zeroBasedIndex >= static_cast<int>(plans.size()))
        {
            std::cout << "编号无效，超出范围。" << std::endl;
            waitForCliEnter();
            return;
        }

        runCliPearlSimulation(false, plans[zeroBasedIndex]);
    }
}

void runCliTntCalculation()
{
    if (!configureCalculationTarget())
    {
        return;
    }

    const Vector2 destination2D = {
        g_appState.destination.x,
        g_appState.destination.z
    };
    const std::vector<LaunchPlan> plans = solveLaunchPlans(destination2D);

    displaySolutions(plans, destination2D);

    if (plans.empty())
    {
        waitForCliEnter();
        return;
    }

    promptSimulationFromPlans(plans);
}
