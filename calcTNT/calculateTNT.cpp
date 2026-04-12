//
// Created by cT on 2026/4/11.
//
#include <cmath>

#include "calculateTNT.h"
#include "../utils/calculateSingleTNTContribution.h"
#include "assignTNT.h"
#include "../data.h"

Configuration calculateTNT(const vector2 targetPosition, const int consumingTicks)
{
    // 计算目的地相对于珍珠发射点的位移
    const vector2 offset = {
        targetPosition.X - pearl.Position.X,
        targetPosition.Z - pearl.Position.Z
    };

    // 执行版本判断逻辑
    double totalDisplacementWeight;
    if (calculatorConfig.below1_21_1)
    {
        // 计算总位移权重 totalDisplacementWeight (1.21.1 and below)
        totalDisplacementWeight = (1.0 - pow(0.99, consumingTicks)) / (1.0 - 0.99);
    }else
    {
        // 计算总位移权重 totalDisplacementWeight (1.21.1 and above)
        totalDisplacementWeight = 0.99 * (1.0 - pow(0.99, consumingTicks)) / (1.0 - 0.99);
    }

    // 计算目标速度
    const auto [X, Z] = offset / totalDisplacementWeight;

    // 计算所需速度
    const vector2 requiredVelocity = {
        X - pearl.Motion.X,
        Z - pearl.Motion.Z
    };

    // 计算推力矢量的大小（模长）
    const vector3 singleTNTMotion = calculateSingleTNTContribution(tntPosition.NorthEast, pearl.Position);
    const double length = singleTNTMotion.lengthHorizontal();

    // C1 和 C2 代表两个 45 度轴向上的 TNT 需求系数
    const double C1 = (-requiredVelocity.X + requiredVelocity.Z) / (sqrt(2.0) * length);
    const double C2 = (requiredVelocity.X + requiredVelocity.Z) / (sqrt(2.0) * length);

    // 解得炮膛布局
    barrelLayout = {
        std::max(C1, 0.0),
        std::max(C2, 0.0),
        std::max(-C2, 0.0),
        std::max(-C1, 0.0)
    };

    // 返回分配后的TNT与方向
    return assignTNT(barrelLayout);
}
