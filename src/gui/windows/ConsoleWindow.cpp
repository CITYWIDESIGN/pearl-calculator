#include "ConsoleWindow.h"

#include <imgui.h>

#include "../app/AppContext.h"
#include "../i18n/I18n.h"
#include "../theme/Theme.h"

namespace pearl::gui
{
    namespace
    {
        ImVec4 colorFor(widgets::ToastKind k)
        {
            const auto& p = currentPalette();
            switch (k)
            {
                case widgets::ToastKind::Success: return p.success;
                case widgets::ToastKind::Warning: return p.warning;
                case widgets::ToastKind::Error:   return p.danger;
                case widgets::ToastKind::Info:
                default:                           return p.textSecondary;
            }
        }
    }

    void ConsoleWindow::render(bool* open)
    {
        if (!ImGui::Begin(t("console.title"), open))
        {
            ImGui::End();
            return;
        }

        ImFont* mono = m_ctx.fonts().defaultMono();
        if (mono) ImGui::PushFont(mono);

        ImGui::BeginChild("console_scroll", ImVec2(0, 0), false,
                          ImGuiWindowFlags_HorizontalScrollbar);
        const auto& entries = m_ctx.consoleEntries();
        for (const auto& e : entries)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, currentPalette().textMuted);
            ImGui::Text("[%s]", e.time.c_str());
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Text, colorFor(e.kind));
            ImGui::TextUnformatted(e.text.c_str());
            ImGui::PopStyleColor();
        }
        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 4.0f)
            ImGui::SetScrollHereY(1.0f);
        ImGui::EndChild();

        if (mono) ImGui::PopFont();

        ImGui::End();
    }
}
