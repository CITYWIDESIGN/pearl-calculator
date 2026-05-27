#include "AboutWindow.h"

#include <imgui.h>

#include "../app/AppContext.h"
#include "../i18n/I18n.h"
#include "../theme/Theme.h"
#include "../widgets/Card.h"
#include "../../data.h"

namespace pearl::gui
{
    void AboutWindow::render(bool* open)
    {
        ImGui::SetNextWindowSize(ImVec2(420, 320), ImGuiCond_Appearing);
        if (!ImGui::Begin(t("about.title"), open))
        {
            ImGui::End();
            return;
        }

        const auto& p = currentPalette();

        widgets::BeginCard("about_card");
        ImGui::PushStyleColor(ImGuiCol_Text, p.accent);
        ImGui::PushFont(nullptr);
        ImGui::TextUnformatted("Pearl Calculator");
        ImGui::PopFont();
        ImGui::PopStyleColor();

        widgets::SoftDivider();

        ImGui::PushStyleColor(ImGuiCol_Text, p.textSecondary);
        ImGui::TextWrapped("%s", t("about.body1"));
        ImGui::TextWrapped("%s", t("about.body2"));
        ImGui::TextWrapped("%s", t("about.body3"));
        ImGui::PopStyleColor();

        widgets::SoftDivider();

        ImGui::Text(t("about.config_ver"), g_appState.version.c_str());
        widgets::EndCard();

        ImGui::End();
    }
}
