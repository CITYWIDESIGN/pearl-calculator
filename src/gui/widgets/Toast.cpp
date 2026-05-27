#include "Toast.h"

#include <algorithm>
#include <utility>

#include "../animation/Easing.h"
#include "../theme/Theme.h"

namespace pearl::gui::widgets
{
    namespace
    {
        ImVec4 colorFor(ToastKind kind)
        {
            const auto& p = currentPalette();
            switch (kind)
            {
                case ToastKind::Success: return p.success;
                case ToastKind::Warning: return p.warning;
                case ToastKind::Error:   return p.danger;
                case ToastKind::Info:
                default:                 return p.accent;
            }
        }
    }

    void ToastQueue::push(std::string text, ToastKind kind, double seconds)
    {
        m_toasts.push_back({std::move(text), kind, seconds, 0.0});
        if (m_toasts.size() > 6) m_toasts.pop_front();
    }

    void ToastQueue::render(double deltaTime)
    {
        if (m_toasts.empty()) return;

        ImGuiViewport* vp = ImGui::GetMainViewport();
        const float margin = 18.0f;
        const float width = 320.0f;
        float yCursor = vp->WorkPos.y + vp->WorkSize.y - margin;

        for (auto it = m_toasts.rbegin(); it != m_toasts.rend(); ++it)
        {
            it->age += deltaTime;
            it->remainingSeconds -= deltaTime;

            const double inT  = std::min(1.0, it->age / 0.35);
            const double outT = it->remainingSeconds < 0.5
                                    ? std::max(0.0, it->remainingSeconds / 0.5)
                                    : 1.0;
            const float ease  = static_cast<float>(easing::easeOutExpo(inT) * easing::easeOutCubic(outT));

            const float alpha = ease;
            const float slide = (1.0f - static_cast<float>(easing::easeOutExpo(inT))) * 24.0f;

            const float h = 56.0f;
            const ImVec2 pos(vp->WorkPos.x + vp->WorkSize.x - margin - width + slide,
                             yCursor - h);
            yCursor -= h + 8.0f;

            const auto& palette = currentPalette();
            ImVec4 bg = palette.surfaceElevated; bg.w = 0.95f * alpha;
            ImVec4 border = palette.border;       border.w *= alpha;
            ImVec4 accent = colorFor(it->kind);   accent.w = alpha;
            ImVec4 text   = palette.textPrimary;  text.w = alpha;

            ImDrawList* dl = ImGui::GetForegroundDrawList(vp);
            const float r = ImGui::GetStyle().WindowRounding;
            dl->AddRectFilled(pos, ImVec2(pos.x + width, pos.y + h),
                              ImGui::ColorConvertFloat4ToU32(bg), r);
            dl->AddRect(pos, ImVec2(pos.x + width, pos.y + h),
                        ImGui::ColorConvertFloat4ToU32(border), r);
            dl->AddRectFilled(pos, ImVec2(pos.x + 4.0f, pos.y + h),
                              ImGui::ColorConvertFloat4ToU32(accent), r);
            dl->AddText(ImVec2(pos.x + 16.0f, pos.y + 18.0f),
                        ImGui::ColorConvertFloat4ToU32(text), it->text.c_str());
        }

        while (!m_toasts.empty() && m_toasts.front().remainingSeconds <= 0.0)
            m_toasts.pop_front();
    }
}
