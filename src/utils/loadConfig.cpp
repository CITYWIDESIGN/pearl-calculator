//
// Created by cT on 2026/4/9.
//

#include "loadConfig.h"

#include <filesystem>
#include <fstream>
#include <iostream>

#include "../data.h"
#include <nlohmann/json.hpp>
#include "createConfig.h"

using json = nlohmann::json;
namespace fs = std::filesystem;

void loadConfiguration(const std::string& path)
{
    const std::string fullPath = path + "/config.json";

    // 在配置缺失时先生成默认文件。
    if (!fs::exists(fullPath))
    {
        createDefaultConfiguration(path);
    }

    // 打开配置文件。
    std::ifstream file(fullPath);
    if (!file.is_open())
    {
        std::cerr << "Error: Unable to open configuration file." << std::endl;
        return;
    }

    try
    {
        json configJson;
        file >> configJson;

        // 读取基础配置。
        g_appState.version = configJson["Version"];
        g_appState.maxTnt = configJson["MaxTNT"];

        // 读取 TNT 位置。
        g_appState.tntPositions.northEast = {
            configJson["NorthEastTNT"]["X"].get<double>(),
            configJson["NorthEastTNT"]["Y"].get<double>(),
            configJson["NorthEastTNT"]["Z"].get<double>()
        };
        g_appState.tntPositions.northWest = {
            configJson["NorthWestTNT"]["X"].get<double>(),
            configJson["NorthWestTNT"]["Y"].get<double>(),
            configJson["NorthWestTNT"]["Z"].get<double>()
        };
        g_appState.tntPositions.southEast = {
            configJson["SouthEastTNT"]["X"].get<double>(),
            configJson["SouthEastTNT"]["Y"].get<double>(),
            configJson["SouthEastTNT"]["Z"].get<double>()
        };
        g_appState.tntPositions.southWest = {
            configJson["SouthWestTNT"]["X"].get<double>(),
            configJson["SouthWestTNT"]["Y"].get<double>(),
            configJson["SouthWestTNT"]["Z"].get<double>()
        };

        // 读取珍珠偏移。
        g_appState.pearlOffset.x = configJson["Offset"]["X"].get<double>();
        g_appState.pearlOffset.z = configJson["Offset"]["Z"].get<double>();

        // 读取珍珠初始状态。
        g_appState.pearl.position = {
            configJson["Pearl"]["Position"]["X"].get<double>(),
            configJson["Pearl"]["Position"]["Y"].get<double>(),
            configJson["Pearl"]["Position"]["Z"].get<double>()
        };
        g_appState.pearl.motion = {
            configJson["Pearl"]["Motion"]["X"].get<double>(),
            configJson["Pearl"]["Motion"]["Y"].get<double>(),
            configJson["Pearl"]["Motion"]["Z"].get<double>()
        };

        // 读取默认方向与 ROM 槽位配置。
        g_appState.defaultRedTntDirection = configJson["DefaultRedTNTDirection"].get<std::string>();
        g_appState.defaultBlueTntDirection = configJson["DefaultBlueTNTDirection"].get<std::string>();
        g_appState.redSlots = configJson["RedSlots"].get<std::vector<int>>();
        g_appState.blueSlots = configJson["BlueSlots"].get<std::vector<int>>();
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Error: Failed to read configuration." << exception.what() << std::endl;
    }
}