#ifndef PEARLCALCULATOR_GUI_THEME_H
#define PEARLCALCULATOR_GUI_THEME_H

#include <imgui.h>

namespace pearl::gui
{
    struct ThemePalette
    {
        ImVec4 background;          // 主窗口底色
        ImVec4 surface;             // 卡片底色
        ImVec4 surfaceElevated;     // 悬浮卡片底色
        ImVec4 border;              // 细边线
        ImVec4 divider;             // 分割线

        ImVec4 textPrimary;
        ImVec4 textSecondary;
        ImVec4 textMuted;

        ImVec4 accent;              // 主强调色（冷色调）
        ImVec4 accentHover;
        ImVec4 accentActive;

        ImVec4 success;
        ImVec4 warning;
        ImVec4 danger;

        ImVec4 grid;                // Minecraft 风格网格主色
        ImVec4 gridSubtle;          // 网格次色
        ImVec4 trajectory;          // 轨迹折线色
    };

    // 把现代化深色主题应用到 ImGui 全局样式。
    void applyDarkTheme(float cornerRadius, float windowOpacity);

    // 把现代化亮色主题应用到 ImGui 全局样式（用于切换）。
    void applyLightTheme(float cornerRadius, float windowOpacity);

    // 当前活动调色板（供 widgets / renderer 使用）。
    const ThemePalette& currentPalette();
}

#endif
