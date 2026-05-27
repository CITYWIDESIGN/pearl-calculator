#ifndef PEARLCALCULATOR_GUI_WIDGETS_CARD_H
#define PEARLCALCULATOR_GUI_WIDGETS_CARD_H

#include <imgui.h>

namespace pearl::gui { class Animator; }

namespace pearl::gui::widgets
{
    // 一个带圆角 / 阴影感 / 内边距的卡片容器。
    // 用法：
    //   if (BeginCard("config", ImVec2(0, 0))) { ... ; EndCard(); }
    bool BeginCard(const char* id,
                   const ImVec2& size = ImVec2(0, 0),
                   bool elevated = false,
                   float padding = 14.0f);
    void EndCard();

    // 卡片标题行：左侧粗体标题 + 右侧可选副文字。
    void CardHeader(const char* title, const char* subtitle = nullptr);

    // 细分割线。
    void SoftDivider(float verticalPadding = 6.0f);

    // 标签：一个柔和背景的小胶囊。
    void Pill(const char* text, ImVec4 color);

    // 带弹出动画的下拉框；返回 true 表示用户改变了选项。
    // - 弹出时：内容 alpha 0→1 + translateY 6px→0（OutCubic 180ms）
    // - 收起时：ImGui 自身的关闭立即生效（popup 关闭瞬间不再绘制）
    bool AnimatedCombo(const char* id,
                       const char* const* items,
                       int itemCount,
                       int* selectedIndex,
                       Animator& animator,
                       const char* preview = nullptr);
}

#endif
