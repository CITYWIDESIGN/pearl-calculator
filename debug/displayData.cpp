//
// Created by cT on 2026/4/9.
//
#include <iostream>
#include <iomanip>
#include <string>

#include "displayData.h"
#include "../data.h"

// 格式化辅助
std::string f(const double val) {
    std::string s = std::to_string(val);
    s.erase(s.find_last_not_of('0') + 1, std::string::npos);
    if (s.back() == '.') s += '0';
    return s;
}

void displayAllConfigData() {
    std::cout << "--- 珍珠计算器 配置数据 ---" << std::endl;

    // 基础信息
    std::cout << "版本: " << version << std::endl;
    std::cout << "最大当量: " << maxTNT << std::endl;
    std::cout << "默认落点: 红" << defaultRedTNTDirection << " / 蓝" << defaultBlueTNTDirection << std::endl;
    std::cout << std::endl;

    // TNT 坐标
    std::cout << "TNT 坐标配置 (高精度):" << std::endl;

    std::cout.unsetf(std::ios::fixed);
    std::cout << std::setprecision(20);

    auto p = [](const std::string& name, const vector3& v) {
        std::cout << name << ":" << std::endl;
        std::cout << "X: " << v.X << "\tY: " << v.Y << "\tZ: " << v.Z << std::endl;
    };

    p("东北", tntPosition.NorthEast);
    p("西北", tntPosition.NorthWest);
    p("东南", tntPosition.SouthEast);
    p("西南", tntPosition.SouthWest);
    std::cout << std::endl;

    // 珍珠信息
    std::cout << "珍珠位置: " << f(pearl.Position.X) << ", " << f(pearl.Position.Y) << ", " << f(pearl.Position.Z) << std::endl;
    std::cout << "珍珠动量: " << f(pearl.Motion.X) << ", " << f(pearl.Motion.Y) << ", " << f(pearl.Motion.Z) << std::endl;
    std::cout << "珍珠偏移: X " << f(pearlOffset.X) << " / Z " << f(pearlOffset.Z) << std::endl;
    std::cout << std::endl;

    // ROM 排版
    auto printSlots = [](const std::string& name, const std::vector<int>& slots) {
        std::cout << name << "ROM排版: ";
        for (size_t i = 0; i < slots.size(); ++i) {
            std::cout << slots[i] << (i == slots.size() - 1 ? "" : ", ");
        }
        std::cout << std::endl;
    };

    printSlots("红色", redSlots);
    printSlots("蓝色", blueSlots);

    std::cout << "---------------------------" << std::endl;
}