//
// Created by cT on 2026/4/9.
//

#ifndef PEARLCALCULATOR_DATA_H
#define PEARLCALCULATOR_DATA_H

#include <string>
#include <vector>
#include <cmath>

// 基础坐标结构
struct vector3
{
    double X, Y, Z;

    // 分量加法
    vector3 operator+(const vector3& other) const
    {
        return {X + other.X, Y + other.Y, Z + other.Z};
    }

    // 分量减法
    vector3 operator-(const vector3& other) const
    {
        return {X - other.X, Y - other.Y, Z - other.Z};
    }

    // 分量乘法
    vector3 operator*(const vector3& other) const
    {
        return {X * other.X, Y * other.Y, Z * other.Z};
    }

    // 标量乘法
    vector3 operator*(const double scalar) const
    {
        return {X * scalar, Y * scalar, Z * scalar};
    }

    // 分量除法
    vector3 operator/(const vector3& other) const
    {
        return {X / other.X, Y / other.Y, Z / other.Z};
    }

    // 标量除法
    vector3 operator/(const double scalar) const
    {
        return {X / scalar, Y / scalar, Z / scalar};
    }

    // 全空间模长
    [[nodiscard]] double length() const
    {
        return std::sqrt(X * X + Y * Y + Z * Z);
    }

    // 水平空间模长
    [[nodiscard]] double lengthHorizontal() const
    {
        return std::sqrt(X * X + Z * Z);
    }
};

struct vector2
{
    double X, Z;

    // 分量加法
    vector2 operator+(const vector2& other) const
    {
        return {X + other.X, Z + other.Z};
    }

    // 分量减法
    vector2 operator-(const vector2& other) const
    {
        return {X - other.X, Z - other.Z};
    }

    // 分量乘法
    vector2 operator*(const vector2& other) const
    {
        return {X * other.X, Z * other.Z};
    }

    // 标量乘法
    vector2 operator*(const double scalar) const
    {
        return {X * scalar, Z * scalar};
    }

    // 分量除法
    vector2 operator/(const vector2& other) const
    {
        return {X / other.X, Z / other.Z};
    }

    // 标量除法
    vector2 operator/(const double scalar) const
    {
        return {X / scalar, Z / scalar};
    }

    // 水平空间模长
    [[nodiscard]] double lengthHorizontal() const
    {
        return std::sqrt(X * X + Z * Z);
    }
};

// TNT方位结构
struct TNTPos
{
    vector3 NorthEast;
    vector3 NorthWest;
    vector3 SouthEast;
    vector3 SouthWest;
};

// 珍珠状态结构
struct PearlInfo
{
    vector3 Position;
    vector3 Motion;
};

// 落点配置
struct Configuration
{
    int redTNT;
    int blueTNT;
    std::string direction;
};

// 炮膛布局
struct Layout
{
    double NorthEast;
    double NorthWest;
    double SouthEast;
    double SouthWest;
};

// 计算器配置
struct Config
{
    bool below1_21_1;
    bool accuracyPriority;
    bool specifiesYPosition;
    bool specifiesGameTick;
};

// 珍珠模拟所需返回值
struct Simulate
{
    vector3 Position;
    vector3 Motion;
    int GameTick;
};

// 计算结果
struct Plan
{
    double landingOffset{};
    Configuration Result;
    int arriveGameTick;
};

// --- 全局变量声明 ---

// 物理信息
extern TNTPos tntPosition;
extern vector2 pearlOffset;
extern PearlInfo pearl;
extern Simulate result;

// 基础配置
extern vector3 destination;
extern Configuration configuration;
extern Layout barrelLayout;
extern Config calculatorConfig;
extern std::string version;
extern Plan calculateResult;
extern double targetYPosition;
extern int maxTNT;

// 方向配置
extern std::string defaultRedTNTDirection;
extern std::string defaultBlueTNTDirection;

// ROM槽位数据
extern std::vector<int> redSlots;
extern std::vector<int> blueSlots;

#endif // PEARLCALCULATOR_DATA_H
