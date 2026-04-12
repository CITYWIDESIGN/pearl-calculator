//
// Created by cT on 2026/4/11.
//
#include <string>
#include <iostream>

#include "calculatePearlInitialMotion.h"
#include "../utils/calculateSingleTNTContribution.h"
#include "../data.h"

// 分配不同方向下红蓝TNT的坐标
static void
distributeTNTPosition(const std::string& direction, vector3& redPos, vector3& bluePos)
{
    vector3 positionA{}, positionB{};
    std::string positionAName, positionBName;

    // 根据方向分配TNT
    if (direction == "North")
    {
        positionA = tntPosition.SouthEast;
        positionB = tntPosition.SouthWest;
        positionAName = "SouthEast";
        positionBName = "SouthWest";
    }
    else if (direction == "South")
    {
        positionA = tntPosition.NorthEast;
        positionB = tntPosition.NorthWest;
        positionAName = "NorthEast";
        positionBName = "NorthWest";
    }
    else if (direction == "West")
    {
        positionA = tntPosition.NorthEast;
        positionB = tntPosition.SouthEast;
        positionAName = "NorthEast";
        positionBName = "SouthEast";
    }
    else if (direction == "East")
    {
        positionA = tntPosition.NorthWest;
        positionB = tntPosition.SouthWest;
        positionAName = "NorthWest";
        positionBName = "SouthWest";
    }
    else return;

    // 分配红蓝TNT坐标
    if (positionAName == defaultRedTNTDirection)
    {
        redPos = positionA;
        bluePos = positionB;
    }
    else if (positionBName == defaultRedTNTDirection)
    {
        redPos = positionB;
        bluePos = positionA;
    }
    else if (positionAName == defaultBlueTNTDirection)
    {
        redPos = positionB;
        bluePos = positionA;
    }
    else if (positionBName == defaultBlueTNTDirection)
    {
        redPos = positionA;
        bluePos = positionB;
    }
}

// 计算珍珠初始（第0gt）的 Motion
vector3 calculatePearlInitialMotion(const int redTNTCount, const int blueTNTCount, const std::string& direction)
{
    // 如果没有 TNT 助推 直接返回珍珠原 Motion
    if (redTNTCount == 0 && blueTNTCount == 0)
    {
        return pearl.Motion;
    }

    // 声明红蓝TNT的坐标变量
    vector3 redTNTPosition{}, blueTNTPosition{};

    // 调用 distributeTNTPosition 自动分配红蓝 TNT 坐标
    distributeTNTPosition(direction, redTNTPosition, blueTNTPosition);

    // 计算红色 TNT 提供的 Motion
    const vector3 redTNTMotion = calculateSingleTNTContribution(redTNTPosition, pearl.Position) * redTNTCount;
    // 计算蓝色 TNT 提供的 Motion
    const vector3 blueTNTMotion = calculateSingleTNTContribution(blueTNTPosition, pearl.Position) * blueTNTCount;

    // 返回时加上珍珠原本的 Motion
    return redTNTMotion + blueTNTMotion + pearl.Motion;
}
