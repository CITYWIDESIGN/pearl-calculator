//
// Created by cT on 2026/4/12.
//

#include <iostream>
#include <limits>
#include <vector>
#include <string>
#include <algorithm>
#include <conio.h>

#include "run_cli.h"
#include "../utils/clearScreen.h"
#include "../calcTNT/solve.h"
#include "../debug/displaySolve.h"
#include "../simulationPearl/simulatePearl.h"
#include "../debug/displaySimulateResult.h"
#include "../data.h"

// 清理输入流缓冲区，防止误读
static void clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// 屏幕暂停函数，等待用户确认
static void pause() {
    std::cout << "\n按回车键继续..." << std::endl;
    if (std::cin.peek() == '\n') std::cin.ignore();
    std::cin.get();
}

// 珍珠轨迹模拟核心函数
static void simPearl(const bool isManual, const Plan &autoPlan = {}) {
    Configuration config;
    int ticks = 0;

    // 手动模式：由用户输入 TNT 数量和方向
    if (isManual) {
        std::cout << "请输入 Red TNT 数量: ";
        std::cin >> config.redTNT;
        std::cout << "请输入 Blue TNT 数量: ";
        std::cin >> config.blueTNT;

        // 验证方向输入是否为合法字符串
        while (true) {
            std::cout << "请输入方向 (East, South, West, North): ";
            std::cin >> config.direction;
            if (config.direction == "East" || config.direction == "South" ||
                config.direction == "West" || config.direction == "North") {
                break;
                }
            std::cout << "[错误] 无效方向，请重新输入！" << std::endl;
        }

        std::cout << "请输入模拟时长: ";
        std::cin >> ticks;
        clearInput();
        clearScreen();
    }
    // 自动模式：从计算好的 Plan 中提取参数
    else {
        config = autoPlan.Result;
        ticks = autoPlan.arriveGameTick + 10;
        clearScreen();
    }

    // 打印当前模拟的基础参数
    std::cout << "[珍珠模拟]\n\nRed\t" << config.redTNT
                << "\nBlue\t" << config.blueTNT
                << "\nDir\t" << config.direction
                << std::endl << std::endl;

    // 执行珍珠物理模拟计算
    const std::vector<Simulate> trajectory = simulatePearl(config, ticks);
    // 打印模拟出的轨迹列表
    displaySimulateResult(trajectory);

    // 自动模式下，额外打印珍珠到达目标时的具体时刻
    if (!isManual) {
        for (const auto& point : trajectory) {
            if (point.GameTick == autoPlan.arriveGameTick) {
                std::cout << "\n>>> 珍珠于 " << point.GameTick << " GT 到达目标坐标。" << std::endl;
                break;
            }
        }
    }

    pause();
}

// 计算所需的 TNT 当量
static void calcTNT() {
    std::cout << "[模式选择]\n" << std::endl;
    std::cout << "1. 默认\n2. 指定落点 Y 坐标\n\n请选择模式：";
    int mode = 0;
    std::cin >> mode;
    clearScreen();

    double x = 0, y = 0, z = 0;

    // 模式1：标准计算模式
    if (mode == 1) {
        calculatorConfig.accuracyPriority = false;
        calculatorConfig.specifiesYPosition = false;
        std::cout << "[默认模式]\n" << std::endl;
        std::cout << "目的地 X: ";
        std::cin >> x;
        std::cout << "目的地 Z: ";
        std::cin >> z;
        destination = {x, 0, z};
    }
    // 模式2：高精度 Y 轴定位模式
    else if (mode == 2) {
        calculatorConfig.accuracyPriority = true;
        calculatorConfig.specifiesYPosition = true;
        std::cout << "[指定 Y 坐标模式]\n" << std::endl;
        std::cout << "目的地 X: ";
        std::cin >> x;
        std::cout << "目的地 Y: ";
        std::cin >> y;
        std::cout << "目的地 Z: ";
        std::cin >> z;
        destination = {x, y, z};
    } else {
        std::cout << "暂不支持该模式。" << std::endl;
        pause();
        return;
    }

    targetYPosition = destination.Y;
    // 调用算法求解可能的 TNT 配置方案
    const std::vector<Plan> result_solve = solve({destination.X, destination.Z});
    displaySolve(result_solve, {destination.X, destination.Z});

    // 如果未找到可行方案，直接返回
    if (result_solve.empty()) {
        pause();
        return;
    }

    // 提供方案选择并支持进入轨迹模拟过程
    std::cout << "按 [S] 开启轨迹模拟，或按其他任意键返回..." << std::endl;
    char key = static_cast<char>(_getch());

    if (key == 's' || key == 'S') {
        std::cout << "请选择要模拟的方案编号 (1-" << result_solve.size() << "): ";
        int planIdx = 0;

        if (std::cin >> planIdx) {
            const int realIdx = planIdx - 1;

            // 检查方案编号范围
            if (realIdx >= 0 && realIdx < static_cast<int>(result_solve.size())) {
                simPearl(false, result_solve[realIdx]);
            } else {
                std::cout << "编号无效，超出范围。" << std::endl;
                pause();
            }
        } else {
            clearInput();
        }
    }
}

// CLI 主循环逻辑
void run_cli() {
    while (true) {
        clearScreen();
        std::cout << "[珍珠计算器]\n" << std::endl;
        std::cout << "1. 计算 TNT 当量\n2. 珍珠模拟\n0. 退出\n\n请选择：";

        int choice = 0;
        if (!(std::cin >> choice)) {
            clearInput();
            continue;
        }

        // 退出程序
        if (choice == 0) break;
        clearScreen();

        // 根据菜单选择进入不同模块
        if (choice == 1) {
            calcTNT();
        } else if (choice == 2) {
            simPearl(true);
        } else {
            std::cout << "无效选择。" << std::endl;
            pause();
        }
    }
}