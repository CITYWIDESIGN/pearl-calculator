//
// Created by cT on 2026/4/10.
//

#include "data.h"

// 变量定义与初始化
TNTPos tntPosition;
vector2 pearlOffset;
PearlInfo pearl;
Simulate result;

vector3 destination;
Configuration configuration;
Layout barrelLayout;
Config calculatorConfig;
std::string version;
Plan calculateResult;
double targetYPosition;
int maxTNT = 0;

std::string defaultRedTNTDirection;
std::string defaultBlueTNTDirection;

std::vector<int> redSlots;
std::vector<int> blueSlots;