//
// Created by cT on 2026/4/9.
//

#include "displayData.h"

#include <iomanip>
#include <iostream>
#include <string>

#include "../data.h"

namespace
{
    std::string formatDouble(const double value)
    {
        std::string formatted = std::to_string(value);
        formatted.erase(formatted.find_last_not_of('0') + 1, std::string::npos);

        if (!formatted.empty() && formatted.back() == '.')
        {
            formatted += '0';
        }

        return formatted;
    }
}

void displayApplicationState()
{
    std::cout << "--- 珍珠计算器 配置数据 ---" << std::endl;
    std::cout << "版本: " << g_appState.version << std::endl;
    std::cout << "最大当量: " << g_appState.maxTnt << std::endl;
    std::cout << "默认落点: 红" << g_appState.defaultRedTntDirection
        << " / 蓝" << g_appState.defaultBlueTntDirection << std::endl;
    std::cout << std::endl;

    // 输出 TNT 坐标信息。
    std::cout << "TNT 坐标配置 (高精度):" << std::endl;
    std::cout.unsetf(std::ios::fixed);
    std::cout << std::setprecision(20);

    const auto printPosition = [](const std::string& name, const Vector3& position)
    {
        std::cout << name << ":" << std::endl;
        std::cout << "X: " << position.x << "\tY: " << position.y << "\tZ: " << position.z << std::endl;
    };

    printPosition("东北", g_appState.tntPositions.northEast);
    printPosition("西北", g_appState.tntPositions.northWest);
    printPosition("东南", g_appState.tntPositions.southEast);
    printPosition("西南", g_appState.tntPositions.southWest);
    std::cout << std::endl;

    // 输出珍珠初始状态。
    std::cout << "珍珠位置: " << formatDouble(g_appState.pearl.position.x) << ", "
        << formatDouble(g_appState.pearl.position.y) << ", "
        << formatDouble(g_appState.pearl.position.z) << std::endl;
    std::cout << "珍珠动量: " << formatDouble(g_appState.pearl.motion.x) << ", "
        << formatDouble(g_appState.pearl.motion.y) << ", "
        << formatDouble(g_appState.pearl.motion.z) << std::endl;
    std::cout << "珍珠偏移: X " << formatDouble(g_appState.pearlOffset.x)
        << " / Z " << formatDouble(g_appState.pearlOffset.z) << std::endl;
    std::cout << std::endl;

    // 输出 ROM 排版信息。
    const auto printSlots = [](const std::string& name, const std::vector<int>& slots)
    {
        std::cout << name << "ROM排版: ";
        for (std::size_t index = 0; index < slots.size(); ++index)
        {
            std::cout << slots[index] << (index == slots.size() - 1 ? "" : ", ");
        }
        std::cout << std::endl;
    };

    printSlots("红色", g_appState.redSlots);
    printSlots("蓝色", g_appState.blueSlots);
    std::cout << "---------------------------" << std::endl;
}