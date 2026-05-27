#include "SettingsWindow.h"

#include <vector>

#include <imgui.h>

#include "../app/AppContext.h"
#include "../i18n/I18n.h"
#include "../theme/Theme.h"
#include "../widgets/AnimatedTabBar.h"
#include "../widgets/Card.h"

namespace pearl::gui
{
    namespace
    {
        // 仅渲染设置内容（不含保存按钮）
        void renderMenuTabBody(AppContext& ctx)
        {
            auto& m = ctx.settings().menu();
            bool dirty = false;

            widgets::BeginCard("menu_appearance");
            {
                widgets::CardHeader(t("settings.section.appearance"),
                                    t("settings.section.appearance_sub"));

                int theme = static_cast<int>(m.theme);
                const char* themeNames[] = {t("settings.theme.dark"), t("settings.theme.light")};
                ImGui::TextUnformatted(t("settings.theme")); ImGui::SameLine();
                ImGui::PushItemWidth(160);
                if (widgets::AnimatedCombo("theme", themeNames, 2, &theme, ctx.animator()))
                {
                    m.theme = static_cast<ThemeMode>(theme);
                    dirty = true;
                }
                ImGui::PopItemWidth();

                int lang = static_cast<int>(m.language);
                const char* langNames[] = {t("settings.lang.en"), t("settings.lang.zh")};
                ImGui::TextUnformatted(t("settings.language")); ImGui::SameLine();
                ImGui::PushItemWidth(160);
                if (widgets::AnimatedCombo("lang", langNames, 2, &lang, ctx.animator()))
                {
                    m.language = static_cast<Language>(lang);
                    dirty = true;
                }
                ImGui::PopItemWidth();

                if (ImGui::SliderFloat(t("settings.corner"),  &m.cornerRadius,  0.0f, 16.0f, "%.1f")) dirty = true;
                if (ImGui::SliderFloat(t("settings.opacity"), &m.windowOpacity, 0.7f, 1.0f, "%.2f")) dirty = true;
            }
            widgets::EndCard();

            ImGui::Dummy(ImVec2(0, 6));

            widgets::BeginCard("menu_typography");
            {
                widgets::CardHeader(t("settings.section.fonts"),
                                    t("settings.section.fonts_sub"));

                static char uiBuf[64];
                static char monoBuf[64];
                static bool init = false;
                if (!init)
                {
                    std::snprintf(uiBuf, sizeof(uiBuf), "%s", m.uiFont.c_str());
                    std::snprintf(monoBuf, sizeof(monoBuf), "%s", m.monoFont.c_str());
                    init = true;
                }
                if (ImGui::InputText(t("settings.font.ui"),   uiBuf,   sizeof(uiBuf)))   { m.uiFont   = uiBuf;   dirty = true; }
                if (ImGui::InputText(t("settings.font.mono"), monoBuf, sizeof(monoBuf))) { m.monoFont = monoBuf; dirty = true; }
                if (ImGui::SliderFloat(t("settings.font.size"), &m.fontSize, 12.0f, 28.0f, "%.0f")) dirty = true;
                if (ImGui::SliderFloat(t("settings.ui_scale"),  &m.uiScale,  0.8f,  1.6f, "%.2f")) dirty = true;
                ImGui::PushStyleColor(ImGuiCol_Text, currentPalette().textMuted);
                ImGui::TextUnformatted(t("settings.font.note"));
                ImGui::PopStyleColor();
            }
            widgets::EndCard();

            ImGui::Dummy(ImVec2(0, 6));

            widgets::BeginCard("menu_motion");
            {
                widgets::CardHeader(t("settings.section.motion"),
                                    t("settings.section.motion_sub"));
                if (ImGui::SliderFloat(t("settings.anim_speed"), &m.animationSpeed, 0.1f, 3.0f, "x %.2f")) dirty = true;
                ImGui::Checkbox(t("settings.save_layout"), &m.saveLayout);
            }
            widgets::EndCard();

            if (dirty) ctx.applyMenuSettings();
        }

        void renderCalcTabBody(AppContext& ctx)
        {
            auto& c = ctx.settings().calc();

            widgets::BeginCard("calc_engine");
            {
                widgets::CardHeader(t("settings.section.engine"),
                                    t("settings.section.engine_sub"));
                ImGui::SliderInt(t("settings.extra_ticks"), &c.extraSimulationTicks, 0, 200);
                ImGui::Checkbox(t("settings.below_1_21_1"), &c.below1_21_1);
                ImGui::Checkbox(t("settings.accuracy"),     &c.accuracyPriority);
            }
            widgets::EndCard();

            ctx.settings().applyCalculationToAppState();
        }

        // 简洁的"保存"按钮 footer，主操作色 + 居中
        void renderSaveFooter(AppContext& ctx, bool isCalcTab)
        {
            const auto& p = currentPalette();
            const float btnW = 160.0f;
            const float btnH = 34.0f;

            // 顶部分割线
            const ImVec2 cs = ImGui::GetCursorScreenPos();
            const float fullW = ImGui::GetContentRegionAvail().x;
            ImGui::GetWindowDrawList()->AddLine(
                ImVec2(cs.x, cs.y),
                ImVec2(cs.x + fullW, cs.y),
                ImGui::ColorConvertFloat4ToU32(p.divider), 1.0f);
            ImGui::Dummy(ImVec2(0, 8));

            // 居中放置按钮
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (fullW - btnW) * 0.5f);

            ImGui::PushStyleColor(ImGuiCol_Button,        p.accent);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, p.accentHover);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,  p.accentActive);
            ImGui::PushStyleColor(ImGuiCol_Text,          ImVec4(1, 1, 1, 1));
            if (ImGui::Button(t("settings.save_btn"), ImVec2(btnW, btnH)))
            {
                ctx.settings().save();
                ctx.emitLog(isCalcTab ? t("toast.calc_saved") : t("toast.gui_saved"),
                            widgets::ToastKind::Success);
            }
            ImGui::PopStyleColor(4);
        }
    }

    void SettingsWindow::render(bool* open)
    {
        if (!ImGui::Begin(t("settings.title"), open))
        {
            ImGui::End();
            return;
        }

        const std::vector<const char*> tabs = {t("settings.tab.menu"), t("settings.tab.calc")};
        m_activeTab = widgets::AnimatedTabBar("settings_tabs", tabs, m_activeTab, m_ctx.animator());

        // 给底部 sticky footer 预留空间
        const float footerH = 56.0f; // 分割线 + 间距 + 按钮 + 间距
        ImGui::BeginChild("settings_body",
                          ImVec2(0, -footerH),
                          false);
        if (m_activeTab == 0) renderMenuTabBody(m_ctx);
        else                   renderCalcTabBody(m_ctx);
        ImGui::EndChild();

        renderSaveFooter(m_ctx, m_activeTab == 1);

        ImGui::End();
    }
}
