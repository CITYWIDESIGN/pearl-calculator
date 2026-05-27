//
// Created by cT on 2026/4/10.
//

#include <windows.h>
#include <fcntl.h>
#include <io.h>
#include <cstdio>

#include "initialize.h"
#include "loadConfig.h"
#include "../data.h"

namespace
{
    constexpr const char* kConfigDirectory = "../config";
}

void initializeApplication()
{
    // 控制台输入/输出编码全部切到 UTF-8，避免中文乱码。
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    // 让 stdout / stderr 走二进制 UTF-8，否则 cmd 默认 GBK 仍会乱码。
    std::setvbuf(stdout, nullptr, _IONBF, 0);
    std::setvbuf(stderr, nullptr, _IONBF, 0);

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