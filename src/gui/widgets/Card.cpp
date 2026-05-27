#include "Card.h"

#include <string>

#include <imgui_internal.h>

#include "../animation/Animator.h"
#include "../theme/Theme.h"

namespace pearl::gui::widgets
{
    bool BeginCard(const char* id, const ImVec2& size, bool elevated, float padding)
    {
        const auto& p = currentPalette();
        ImVec4 bg = elevated ? p.surfaceElevated : p.surface;

        ImGui::PushStyleColor(ImGuiCol_ChildBg, bg);
        ImGui::PushStyleColor(ImGuiCol_Border, p.border);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, ImGui::GetStyle().WindowRounding);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(padding, padding));

        // ImGui 1.92 起，BeginChild 不论返回什么都必须配对 EndChild。
        // 我们让 EndCard 永远调用，所以这里直接吞掉返回值，对外恒为 true。
        ImGui::BeginChild(id, size, ImGuiChildFlags_Borders | ImGuiChildFlags_AlwaysUseWindowPadding);
        return true;
    }

    void EndCard()
    {
        ImGui::EndChild();
        ImGui::PopStyleVar(3);
        ImGui::PopStyleColor(2);
    }

    void CardHeader(const char* title, const char* subtitle)
    {
        const auto& p = currentPalette();
        ImGui::PushStyleColor(ImGuiCol_Text, p.textPrimary);
        ImGui::TextUnformatted(title);
        ImGui::PopStyleColor();

        if (subtitle && *subtitle)
        {
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Text, p.textMuted);
            ImGui::TextUnformatted(subtitle);
            ImGui::PopStyleColor();
        }
        SoftDivider(6.0f);
    }

    void SoftDivider(float verticalPadding)
    {
        const auto& p = currentPalette();
        ImGui::Dummy(ImVec2(0, verticalPadding));
        ImDrawList* dl = ImGui::GetWindowDrawList();
        const ImVec2 a = ImGui::GetCursorScreenPos();
        const float w = ImGui::GetContentRegionAvail().x;
        dl->AddLine(a, ImVec2(a.x + w, a.y), ImGui::ColorConvertFloat4ToU32(p.divider), 1.0f);
        ImGui::Dummy(ImVec2(0, verticalPadding));
    }

    void Pill(const char* text, ImVec4 color)
    {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        const ImVec2 textSize = ImGui::CalcTextSize(text);
        const ImVec2 pad(10.0f, 4.0f);
        const ImVec2 pos = ImGui::GetCursorScreenPos();
        const ImVec2 size(textSize.x + pad.x * 2, textSize.y + pad.y * 2);

        ImVec4 bg = color; bg.w = 0.18f;
        ImVec4 border = color; border.w = 0.55f;

        dl->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y),
                          ImGui::ColorConvertFloat4ToU32(bg), size.y * 0.5f);
        dl->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y),
                    ImGui::ColorConvertFloat4ToU32(border), size.y * 0.5f);
        dl->AddText(ImVec2(pos.x + pad.x, pos.y + pad.y),
                    ImGui::ColorConvertFloat4ToU32(color), text);

        ImGui::Dummy(size);
    }

    bool AnimatedCombo(const char* id,
                       const char* const* items,
                       int itemCount,
                       int* selectedIndex,
                       Animator& animator,
                       const char* preview)
    {
        if (!selectedIndex || itemCount <= 0) return false;

        ImGui::PushID(id);

        const char* shown = preview ? preview
                                    : (*selectedIndex >= 0 && *selectedIndex < itemCount
                                          ? items[*selectedIndex]
                                          : "");

        // BeginCombo：弹出窗口由 ImGui 内部管理；我们把内容用 alpha + translateY 包起来。
        const std::string aKey = std::string("combo_a_") + id;
        bool changed = false;

        if (ImGui::BeginCombo("##cb", shown))
        {
            // 弹出态：alpha & 偏移由 driveTo 持续推进
            const float a = static_cast<float>(animator.driveTo(
                aKey, 1.0, 0.18, Easing::OutCubic, 0.0));
            const float dy = (1.0f - a) * 6.0f; // 6px → 0

            // 用 PushStyleVar(Alpha) 让所有子项在弹窗内带 alpha
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, a);

            // translateY：把游标向下挪一点，配合 alpha 像"轻微落下"出现
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + dy);

            for (int i = 0; i < itemCount; ++i)
            {
                const bool isSel = (*selectedIndex == i);
                if (ImGui::Selectable(items[i], isSel))
                {
                    *selectedIndex = i;
                    changed = true;
                }
                if (isSel) ImGui::SetItemDefaultFocus();
            }

            ImGui::PopStyleVar();
            ImGui::EndCombo();
        }
        else
        {
            // 关闭态：把 tween 立即拉回 0，下次再开打才会从 0→1 重新动画
            animator.driveTo(aKey, 0.0, 0.0, Easing::Linear, 0.0);
        }

        ImGui::PopID();
        return changed;
    }
}
