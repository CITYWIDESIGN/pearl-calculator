//
// Created by cT on 2026/4/9.
//

#include "loadConfig.h"
#include "../nlohmann/json.hpp"
#include "createConfig.h"
#include "../data.h"

#include <iostream>
#include <fstream>
#include <filesystem>

using json = nlohmann::json;
namespace fs = std::filesystem;

void loadConfig(const std::string& path)
{
    std::string fullPath = path + "/config.json";

    // 检查文件是否存在
    if (!fs::exists(fullPath))
    {
        createConfig(path);
    }

    // 尝试打开文件
    std::ifstream file(fullPath);
    if (!file.is_open())
    {
        std::cerr << "Error: Unable to open configuration file." << std::endl;
        return;
    }

    try
    {
        json j;
        file >> j;

        // 给全局变量赋值
        version = j["Version"];
        maxTNT = j["MaxTNT"];

        // TNT 位置赋值
        tntPosition.NorthEast = {
            j["NorthEastTNT"]["X"].get<double>(),
            j["NorthEastTNT"]["Y"].get<double>(),
            j["NorthEastTNT"]["Z"].get<double>()
        };
        tntPosition.NorthWest = {
            j["NorthWestTNT"]["X"].get<double>(),
            j["NorthWestTNT"]["Y"].get<double>(),
            j["NorthWestTNT"]["Z"].get<double>()
        };
        tntPosition.SouthEast = {
            j["SouthEastTNT"]["X"].get<double>(),
            j["SouthEastTNT"]["Y"].get<double>(),
            j["SouthEastTNT"]["Z"].get<double>()
        };
        tntPosition.SouthWest = {
            j["SouthWestTNT"]["X"].get<double>(),
            j["SouthWestTNT"]["Y"].get<double>(),
            j["SouthWestTNT"]["Z"].get<double>()
        };

        // 珍珠偏移
        pearlOffset.X = j["Offset"]["X"].get<double>();
        pearlOffset.Z = j["Offset"]["Z"].get<double>();

        // 珍珠信息
        pearl.Position = {
            j["Pearl"]["Position"]["X"].get<double>(),
            j["Pearl"]["Position"]["Y"].get<double>(),
            j["Pearl"]["Position"]["Z"].get<double>()
        };
        pearl.Motion = {
            j["Pearl"]["Motion"]["X"].get<double>(),
            j["Pearl"]["Motion"]["Y"].get<double>(),
            j["Pearl"]["Motion"]["Z"].get<double>()
        };

        // 默认方向
        defaultRedTNTDirection = j["DefaultRedTNTDirection"].get<std::string>();
        defaultBlueTNTDirection = j["DefaultBlueTNTDirection"].get<std::string>();

        // ROM模板赋值
        redSlots = j["RedSlots"].get<std::vector<int>>();
        blueSlots = j["BlueSlots"].get<std::vector<int>>();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: Failed to read configuration." << e.what() << std::endl;
    }
}
