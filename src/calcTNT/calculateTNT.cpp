//
// Created by cT on 2026/4/11.
//

#include <cmath>

#include "calculateTNT.h"

#include "../data.h"
#include "../utils/calculateSingleTNTContribution.h"
#include "assignTNT.h"

TntConfiguration calculateTntRequirement(const Vector2 targetPosition, const int travelTicks)
{
    // 计算目标点相对于珍珠发射点的水平位移。
    const Vector2 targetOffset = {
        targetPosition.x - g_appState.pearl.position.x,
        targetPosition.z - g_appState.pearl.position.z
    };

    double totalDisplacementWeight = 0.0;

    // 根据版本差异选择位移权重公式。
    if (g_appState.calculatorSettings.below1_21_1)
    {
        totalDisplacementWeight = (1.0 - std::pow(0.99, travelTicks)) / (1.0 - 0.99);
    }
    else
    {
        totalDisplacementWeight = 0.99 * (1.0 - std::pow(0.99, travelTicks)) / (1.0 - 0.99);
    }

    // 计算命中目标所需的总速度。
    const auto [targetVelocityX, targetVelocityZ] = targetOffset / totalDisplacementWeight;
    const Vector2 requiredImpulse = {
        targetVelocityX - g_appState.pearl.motion.x,
        targetVelocityZ - g_appState.pearl.motion.z
    };

    // 计算单个 TNT 在水平面的推力模长。
    const Vector3 singleTntMotion = calculateSingleTntContribution(
        g_appState.tntPositions.northEast,
        g_appState.pearl.position
    );
    const double singleTntHorizontalStrength = singleTntMotion.horizontalLength();

    // 解算两个 45 度轴上的需求系数。
    const double axisC1 = (-requiredImpulse.x + requiredImpulse.z) / (std::sqrt(2.0) * singleTntHorizontalStrength);
    const double axisC2 = (requiredImpulse.x + requiredImpulse.z) / (std::sqrt(2.0) * singleTntHorizontalStrength);

    // 保存当前炮膛布局权重，供调试或后续扩展使用。
    g_appState.barrelLayout = {
        std::max(axisC1, 0.0),
        std::max(axisC2, 0.0),
        std::max(-axisC2, 0.0),
        std::max(-axisC1, 0.0)
    };

    return assignTntConfiguration(g_appState.barrelLayout);
}
