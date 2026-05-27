#include "AnimatedTabBar.h"

#include <string>

#include <imgui_internal.h>

#include "../animation/Animator.h"
#include "../theme/Theme.h"

namespace pearl::gui::widgets
{
    int AnimatedTabBar(const char* id,
                       const std::vector<const char*>& labels,
                       int selected,
                       Animator& animator,
                       float height)
    {
        if (labels.empty()) return selected;

        const auto& palette = currentPalette();
        ImDrawList* dl = ImGui::GetWindowDrawList();

        ImGui::PushID(id);

        const ImVec2 origin = ImGui::GetCursorScreenPos();
        const float fullWidth = ImGui::GetContentRegionAvail().x;

        std::vector<float> widths;
        widths.reserve(labels.size());
        float total = 0.0f;
        for (const auto* label : labels)
        {
            const ImVec2 ts = ImGui::CalcTextSize(label);
            const float w = ts.x + 28.0f;
            widths.push_back(w);
            total += w;
        }
        // 如果总宽度小于可用区域，把剩余平均分掉，得到"等分铺满"的现代外观。
        if (total < fullWidth)
        {
            const float extra = (fullWidth - total) / static_cast<float>(labels.size());
            for (auto& w : widths) w += extra;
        }

        int hoveredIndex = -1;
        int newSelected = selected;
        float xCursor = origin.x;
        for (int i = 0; i < static_cast<int>(labels.size()); ++i)
        {
            const float w = widths[i];
            const ImVec2 a(xCursor, origin.y);
            const ImVec2 b(xCursor + w, origin.y + height);

            ImGui::SetCursorScreenPos(a);
            ImGui::InvisibleButton(labels[i], ImVec2(w, height));
            const bool hovered = ImGui::IsItemHovered();
            const bool clicked = ImGui::IsItemClicked();
            if (hovered) hoveredIndex = i;
            if (clicked) newSelected = i;

            // hover 背景过渡
            const std::string hoverKey = std::string("hover_") + std::to_string(i);
            const float hoverAmount = static_cast<float>(animator.driveTo(
                hoverKey,
                hovered ? 1.0 : 0.0,
                0.18,
                Easing::OutCubic));
            ImVec4 hoverBg = palette.surfaceElevated;
            hoverBg.w *= hoverAmount * 0.65f;
            if (hoverAmount > 0.001f)
            {
                dl->AddRectFilled(a, b, ImGui::ColorConvertFloat4ToU32(hoverBg),
                                  ImGui::GetStyle().FrameRounding);
            }

            // 文本：选中态用 accent 色 + 主字色，未选中用次字色。
            ImVec4 textColor = i == selected ? palette.textPrimary : palette.textSecondary;
            if (hovered) textColor = palette.textPrimary;
            const ImVec2 ts = ImGui::CalcTextSize(labels[i]);
            const ImVec2 textPos(a.x + (w - ts.x) * 0.5f, a.y + (height - ts.y) * 0.5f);
            dl->AddText(textPos, ImGui::ColorConvertFloat4ToU32(textColor), labels[i]);

            xCursor += w;
        }

        // 底部细分割线
        dl->AddLine(ImVec2(origin.x, origin.y + height),
                    ImVec2(origin.x + fullWidth, origin.y + height),
                    ImGui::ColorConvertFloat4ToU32(palette.divider), 1.0f);

        // 下划线滑动 —— 用相对偏移而非屏幕绝对坐标，避免主窗口被拖动时
        // 缓动器把"旧屏幕位置 → 新屏幕位置"插值掉，导致下划线视觉上乱跑。
        float underlineRel = 0.0f;
        for (int i = 0; i < newSelected && i < static_cast<int>(widths.size()); ++i)
            underlineRel += widths[i];
        const float underlineW = newSelected < static_cast<int>(widths.size()) ? widths[newSelected] : 0.0f;

        const float curRel = static_cast<float>(animator.driveTo(
            std::string("underline_rel_") + id, underlineRel, 0.45, Easing::OutExpo, underlineRel));
        const float curW = static_cast<float>(animator.driveTo(
            std::string("underline_w_") + id, underlineW, 0.45, Easing::OutExpo, underlineW));

        const float curX = origin.x + curRel;

        const float thickness = 2.5f;
        dl->AddRectFilled(
            ImVec2(curX + 8.0f, origin.y + height - thickness),
            ImVec2(curX + curW - 8.0f, origin.y + height),
            ImGui::ColorConvertFloat4ToU32(palette.accent),
            thickness * 0.5f);

        ImGui::SetCursorScreenPos(ImVec2(origin.x, origin.y + height + 6.0f));
        ImGui::PopID();
        return newSelected;
    }
}
