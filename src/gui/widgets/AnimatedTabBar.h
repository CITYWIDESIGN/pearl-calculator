#ifndef PEARLCALCULATOR_GUI_WIDGETS_TAB_BAR_H
#define PEARLCALCULATOR_GUI_WIDGETS_TAB_BAR_H

#include <vector>

#include <imgui.h>

namespace pearl::gui { class Animator; }

namespace pearl::gui::widgets
{
    // 现代化 Tab 条，带下划线滑动 + hover 过渡。
    // 返回值：用户当前活动的 tab 索引（与传入的 selected 同步）。
    int AnimatedTabBar(const char* id,
                       const std::vector<const char*>& labels,
                       int selected,
                       Animator& animator,
                       float height = 36.0f);
}

#endif
