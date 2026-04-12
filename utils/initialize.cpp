//
// Created by cT on 2026/4/10.
//
#include <windows.h>

#include "initialize.h"
#include "loadConfig.h"
#include "../data.h"

#define PATH "../config"

void initialize()
{
    // 中文输出修复
    SetConsoleOutputCP(CP_UTF8);

    // 加载配置文件
    loadConfig(PATH);

    // 将TNT偏移应用到坐标
    tntPosition.NorthEast = {
        pearl.Position.X + tntPosition.NorthEast.X,
        tntPosition.NorthEast.Y,
        pearl.Position.Z + tntPosition.NorthEast.Z
    };
    tntPosition.NorthWest = {
        pearl.Position.X + tntPosition.NorthWest.X,
        tntPosition.NorthWest.Y,
        pearl.Position.Z + tntPosition.NorthWest.Z
    };
    tntPosition.SouthEast = {
        pearl.Position.X + tntPosition.SouthEast.X,
        tntPosition.SouthEast.Y,
        pearl.Position.Z + tntPosition.SouthEast.Z
    };
    tntPosition.SouthWest = {
        pearl.Position.X + tntPosition.SouthWest.X,
        tntPosition.SouthWest.Y,
        pearl.Position.Z + tntPosition.SouthWest.Z
    };

    // 将珍珠偏移应用到坐标
    pearl.Position = {
        pearl.Position.X + pearlOffset.X,
        pearl.Position.Y,
        pearl.Position.Z + pearlOffset.Z
    };

    // 计算器配置
    calculatorConfig = {
        true,
        false,
        true
    };
}
