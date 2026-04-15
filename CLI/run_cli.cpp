//
// Created by cT on 2026/4/12.
//

#include "run_cli.h"

#include <iostream>

#include "../utils/clearScreen.h"
#include "cli_calculation.h"
#include "cli_common.h"
#include "cli_simulation.h"

namespace
{
    void showCliMainMenu()
    {
        clearScreen();
        std::cout << "[珍珠计算器]\n" << std::endl;
        std::cout << "1. 计算 TNT 当量\n2. 珍珠模拟\n0. 退出\n\n请选择：";
    }
}

void runCli()
{
    while (true)
    {
        showCliMainMenu();

        int menuChoice = 0;
        if (!(std::cin >> menuChoice))
        {
            clearCliInputBuffer();
            continue;
        }

        if (menuChoice == 0)
        {
            break;
        }

        clearScreen();

        if (menuChoice == 1)
        {
            runCliTntCalculation();
        }
        else if (menuChoice == 2)
        {
            runCliPearlSimulation(true);
        }
        else
        {
            std::cout << "无效选择。" << std::endl;
            waitForCliEnter();
        }
    }
}
