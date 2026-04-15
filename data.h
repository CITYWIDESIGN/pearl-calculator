//
// Created by cT on 2026/4/9.
//

#ifndef PEARLCALCULATOR_DATA_H
#define PEARLCALCULATOR_DATA_H

#include <cmath>
#include <string>
#include <vector>

// 三维坐标与向量。
struct Vector3
{
    double x{};
    double y{};
    double z{};

    // 执行分量加法。
    Vector3 operator+(const Vector3& other) const
    {
        return {x + other.x, y + other.y, z + other.z};
    }

    // 执行分量减法。
    Vector3 operator-(const Vector3& other) const
    {
        return {x - other.x, y - other.y, z - other.z};
    }

    // 执行分量乘法。
    Vector3 operator*(const Vector3& other) const
    {
        return {x * other.x, y * other.y, z * other.z};
    }

    // 执行标量乘法。
    Vector3 operator*(double scalar) const
    {
        return {x * scalar, y * scalar, z * scalar};
    }

    // 执行分量除法。
    Vector3 operator/(const Vector3& other) const
    {
        return {x / other.x, y / other.y, z / other.z};
    }

    // 执行标量除法。
    Vector3 operator/(double scalar) const
    {
        return {x / scalar, y / scalar, z / scalar};
    }

    // 计算三维模长。
    [[nodiscard]] double length() const
    {
        return std::sqrt(x * x + y * y + z * z);
    }

    // 计算水平模长。
    [[nodiscard]] double horizontalLength() const
    {
        return std::sqrt(x * x + z * z);
    }
};

// 水平二维坐标与向量。
struct Vector2
{
    double x{};
    double z{};

    // 执行分量加法。
    Vector2 operator+(const Vector2& other) const
    {
        return {x + other.x, z + other.z};
    }

    // 执行分量减法。
    Vector2 operator-(const Vector2& other) const
    {
        return {x - other.x, z - other.z};
    }

    // 执行分量乘法。
    Vector2 operator*(const Vector2& other) const
    {
        return {x * other.x, z * other.z};
    }

    // 执行标量乘法。
    Vector2 operator*(double scalar) const
    {
        return {x * scalar, z * scalar};
    }

    // 执行分量除法。
    Vector2 operator/(const Vector2& other) const
    {
        return {x / other.x, z / other.z};
    }

    // 执行标量除法。
    Vector2 operator/(double scalar) const
    {
        return {x / scalar, z / scalar};
    }

    // 计算水平模长。
    [[nodiscard]] double horizontalLength() const
    {
        return std::sqrt(x * x + z * z);
    }
};

// 四个 TNT 槽位的空间坐标。
struct TntPositions
{
    Vector3 northEast;
    Vector3 northWest;
    Vector3 southEast;
    Vector3 southWest;
};

// 珍珠的初始状态。
struct PearlState
{
    Vector3 position;
    Vector3 motion;
};

// 一组红蓝 TNT 与方向配置。
struct TntConfiguration
{
    int redTnt{};
    int blueTnt{};
    std::string direction;
};

// 解析出来的炮膛布局权重。
struct BarrelLayout
{
    double northEast{};
    double northWest{};
    double southEast{};
    double southWest{};
};

// 计算器的运行设置。
struct CalculatorSettings
{
    bool below1_21_1{};
    bool accuracyPriority{};
    bool specifiesYPosition{};
    bool specifiesGameTick{};
};

// 单个游戏刻的模拟结果。
struct SimulationPoint
{
    Vector3 position;
    Vector3 motion;
    int gameTick{};
};

// 一条求解出的候选方案。
struct LaunchPlan
{
    double landingOffset{};
    TntConfiguration configuration;
    int arrivalGameTick{};
};

// 项目级共享状态。
struct AppState
{
    TntPositions tntPositions;
    Vector2 pearlOffset;
    PearlState pearl;

    Vector3 destination;
    BarrelLayout barrelLayout;
    CalculatorSettings calculatorSettings;

    std::string version;
    double targetYPosition{};
    int maxTnt{};

    std::string defaultRedTntDirection;
    std::string defaultBlueTntDirection;

    std::vector<int> redSlots;
    std::vector<int> blueSlots;
};

extern AppState g_appState;

#endif // PEARLCALCULATOR_DATA_H
