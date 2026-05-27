//
// Created by cT on 2026/4/9.
//

#include "createConfig.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include <nlohmann/json.hpp>

using ordered_json = nlohmann::ordered_json;
namespace fs = std::filesystem;

void createDefaultConfiguration(const std::string& path)
{
    const fs::path targetDirectory(path);
    const fs::path configurationPath = targetDirectory / "config.json";

    // 在缺少目录时先创建配置目录。
    if (!path.empty() && !fs::exists(targetDirectory))
    {
        fs::create_directories(targetDirectory);
    }

    // 生成默认配置内容。
    ordered_json configJson;

    configJson["Version"] = "2.7";
    configJson["NorthEastTNT"] = {{"X", 0.8849999904632568}, {"Y", 165.5}, {"Z", -0.8849999904632568}};
    configJson["NorthWestTNT"] = {{"X", -0.7599999904632568}, {"Y", 165.5}, {"Z", -0.8849999904632568}};
    configJson["SouthEastTNT"] = {{"X", 0.8849999904632568}, {"Y", 165.5}, {"Z", 0.7599999904632568}};
    configJson["SouthWestTNT"] = {{"X", -0.7599999904632568}, {"Y", 165.5}, {"Z", 0.7599999904632568}};

    configJson["Destination"] = {{"X", 0.0}, {"Y", 0.0}, {"Z", 0.0}};
    configJson["Offset"] = {{"X", 0.0625}, {"Z", -0.0625}};

    configJson["Pearl"] = {
        {"Position", {{"X", 0.0}, {"Y", 165.347226}, {"Z", 0.0}}},
        {"Motion", {{"X", 0.0}, {"Y", 0.271628}, {"Z", 0.0}}}
    };

    configJson["RedTNT"] = 0;
    configJson["BlueTNT"] = 0;
    configJson["MaxTNT"] = 3360;
    configJson["Direction"] = "North";
    configJson["DefaultRedTNTDirection"] = "NorthEast";
    configJson["DefaultBlueTNTDirection"] = "SouthWest";
    configJson["RedSlots"] = {420, 840, 1680, 1, 2, 3, 4, 10, 20, 40, 80, 110, 150};
    configJson["BlueSlots"] = {150, 110, 80, 40, 20, 10, 4, 3, 2, 1, 1680, 840, 420};

    // 输出配置文件。
    if (std::ofstream file(configurationPath); file.is_open())
    {
        file << configJson.dump(2);
        file.close();
    }
    else
    {
        std::cerr << "Error: Could not create file at " << configurationPath << std::endl;
    }
}