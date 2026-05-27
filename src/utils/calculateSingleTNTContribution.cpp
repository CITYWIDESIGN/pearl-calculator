//
// Created by cT on 2026/4/10.
//

#include "calculateSingleTNTContribution.h"

#include "getDistance3D.h"

Vector3 calculateSingleTntContribution(const Vector3& tntPosition, const Vector3& pearlPosition)
{
    // 计算 TNT 爆心坐标。
    const Vector3 explosionCenter = {
        tntPosition.x,
        tntPosition.y + 0.06125,
        tntPosition.z
    };

    // 计算 TNT 爆心到珍珠本体的距离。
    const double explosionToPearlDistance = calculateDistance3D(explosionCenter, pearlPosition);

    // 根据爆炸衰减计算推力强度。
    const float normalizedStrength = std::max(
        0.0f,
        static_cast<float>((8.0 - explosionToPearlDistance) / 8.0)
    );

    // 计算珍珠眼部坐标。
    const Vector3 pearlEyePosition = {
        pearlPosition.x,
        pearlPosition.y + 0.25f * 0.85f,
        pearlPosition.z
    };

    // 计算 TNT 爆心到珍珠眼部的距离。
    const double explosionToEyeDistance = calculateDistance3D(explosionCenter, pearlEyePosition);

    // 构造爆心指向珍珠眼部的方向向量。
    const Vector3 direction = {
        pearlEyePosition.x - explosionCenter.x,
        pearlEyePosition.y - explosionCenter.y,
        pearlEyePosition.z - explosionCenter.z
    };

    // 避免出现除以零。
    if (explosionToEyeDistance == 0.0)
    {
        return {0.0, 0.0, 0.0};
    }

    // 归一化并缩放方向向量。
    const double scale = normalizedStrength / explosionToEyeDistance;

    return {
        direction.x * scale,
        direction.y * scale,
        direction.z * scale
    };
}