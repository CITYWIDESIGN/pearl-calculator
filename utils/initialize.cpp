//
// Created by cT on 2026/4/10.
//

#include <windows.h>

#include "initialize.h"
#include "loadConfig.h"
#include "../data.h"

namespace
{
    constexpr const char* kConfigDirectory = "../config";
}

void initializeApplication()
{
    // 修正 Windows 控制台的 UTF-8 输出。
    SetConsoleOutputCP(CP_UTF8);

    // 加载外部配置。
    loadConfiguration(kConfigDirectory);

    // 将 TNT 相对偏移转换为世界坐标。
    g_appState.tntPositions.northEast = {
        g_appState.pearl.position.x + g_appState.tntPositions.northEast.x,
        g_appState.tntPositions.northEast.y,
        g_appState.pearl.position.z + g_appState.tntPositions.northEast.z
    };
    g_appState.tntPositions.northWest = {
        g_appState.pearl.position.x + g_appState.tntPositions.northWest.x,
        g_appState.tntPositions.northWest.y,
        g_appState.pearl.position.z + g_appState.tntPositions.northWest.z
    };
    g_appState.tntPositions.southEast = {
        g_appState.pearl.position.x + g_appState.tntPositions.southEast.x,
        g_appState.tntPositions.southEast.y,
        g_appState.pearl.position.z + g_appState.tntPositions.southEast.z
    };
    g_appState.tntPositions.southWest = {
        g_appState.pearl.position.x + g_appState.tntPositions.southWest.x,
        g_appState.tntPositions.southWest.y,
        g_appState.pearl.position.z + g_appState.tntPositions.southWest.z
    };

    // 将珍珠偏移应用到发射点。
    g_appState.pearl.position = {
        g_appState.pearl.position.x + g_appState.pearlOffset.x,
        g_appState.pearl.position.y,
        g_appState.pearl.position.z + g_appState.pearlOffset.z
    };

    // 设置计算器默认模式。
    g_appState.calculatorSettings = {
        true,
        false,
        true,
        false
    };
}