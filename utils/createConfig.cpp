//
// Created by cT on 2026/4/9.
//

#include "createConfig.h"
#include "../nlohmann/json.hpp"

#include <fstream>
#include <string>
#include <filesystem>
#include <iostream>

using ordered_json = nlohmann::ordered_json;
namespace fs = std::filesystem;

void createConfig(const std::string& path)
{
    const fs::path targetDir(path);
    const fs::path filePath = targetDir / "config.json";

    // 如果文件夹不存在 先递归创建文件夹
    if (!path.empty() && !fs::exists(targetDir))
    {
        fs::create_directories(targetDir);
    }

    // 填入数据
    ordered_json j;

    j["Version"] = "2.7";
    j["NorthEastTNT"] = {{"X", 0.8849999904632568}, {"Y", 165.5}, {"Z", -0.8849999904632568}};
    j["NorthWestTNT"] = {{"X", -0.7599999904632568}, {"Y", 165.5}, {"Z", -0.8849999904632568}};
    j["SouthEastTNT"] = {{"X", 0.8849999904632568}, {"Y", 165.5}, {"Z", 0.7599999904632568}};
    j["SouthWestTNT"] = {{"X", -0.7599999904632568}, {"Y", 165.5}, {"Z", 0.7599999904632568}};

    j["Destination"] = {{"X", 0.0}, {"Y", 0.0}, {"Z", 0.0}};
    j["Offset"] = {{"X", 0.0625}, {"Z", -0.0625}};

    j["Pearl"] = {
        {"Position", {{"X", 0.0}, {"Y", 165.347226}, {"Z", 0.0}}},
        {"Motion", {{"X", 0.0}, {"Y", 0.271628}, {"Z", 0.0}}}
    };

    j["RedTNT"] = 0;
    j["BlueTNT"] = 0;
    j["MaxTNT"] = 3360;
    j["Direction"] = "North";
    j["DefaultRedTNTDirection"] = "NorthEast";
    j["DefaultBlueTNTDirection"] = "SouthWest";

    // 数组
    j["RedSlots"] = {420, 840, 1680, 1, 2, 3, 4, 10, 20, 40, 80, 110, 150};
    j["BlueSlots"] = {150, 110, 80, 40, 20, 10, 4, 3, 2, 1, 1680, 840, 420};

    // 保存文件
    if (std::ofstream file(filePath); file.is_open())
    {
        file << j.dump(2);
        file.close();
    }
    else
    {
        std::cerr << "Error: Could not create file at " << filePath << std::endl;
    }
}
