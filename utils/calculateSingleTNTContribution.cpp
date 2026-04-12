//
// Created by cT on 2026/4/10.
//

#include "calculateSingleTNTContribution.h"
#include "../data.h"
#include "getDistance3D.h"

vector3 calculateSingleTNTContribution(const vector3& TNTPos, const vector3& pearlPos)
{
    // TNT的爆心
    const vector3 TNTExplosionCenter = {
        TNTPos.X,
        TNTPos.Y + 0.06125,
        TNTPos.Z
    };

    // TNT爆心与珍珠之间的距离
    double explosionToPearlDistance = getDistance3D(TNTExplosionCenter, pearlPos);

    // 向量归一化
    const float v0 = std::max(0.0f, static_cast<float>((8.0 - explosionToPearlDistance) / 8.0));

    // 珍珠眼部坐标
    const vector3 pearlEyePosition = {
        pearlPos.X,
        pearlPos.Y + 0.25f * 0.85f,
        pearlPos.Z
    };

    // TNT爆心到珍珠眼部之间的距离
    const double explosionToEyeDistance = getDistance3D(TNTExplosionCenter, pearlEyePosition);

    // 方向向量
    const vector3 direction = {
        pearlEyePosition.X - TNTExplosionCenter.X,
        pearlEyePosition.Y - TNTExplosionCenter.Y,
        pearlEyePosition.Z - TNTExplosionCenter.Z
    };

    // 防止零值
    if (explosionToEyeDistance == 0.0)
    {
        return {0, 0, 0};
    }

    // 单位化
    const double scale = v0 / explosionToEyeDistance;

    return {
        direction.X * scale,
        direction.Y * scale,
        direction.Z * scale
    };
}
