#include "Theme.h"

namespace pearl::gui
{
    namespace
    {
        ThemePalette g_palette;
        bool g_isLight = false;

        ImVec4 rgba(int r, int g, int b, float a = 1.0f)
        {
            return ImVec4(r / 255.0f, g / 255.0f, b / 255.0f, a);
        }

        void buildDarkPalette()
        {
            g_palette.background       = rgba(15, 17, 22);
            g_palette.surface          = rgba(22, 25, 32);
            g_palette.surfaceElevated  = rgba(28, 32, 41);
            g_palette.border           = rgba(45, 51, 64);
            g_palette.divider          = rgba(34, 38, 48);

            g_palette.textPrimary      = rgba(225, 230, 240);
            g_palette.textSecondary    = rgba(170, 178, 196);
            g_palette.textMuted        = rgba(110, 118, 134);

            g_palette.accent           = rgba(86, 156, 214);
            g_palette.accentHover      = rgba(122, 178, 230);
            g_palette.accentActive     = rgba(64, 130, 190);

            g_palette.success          = rgba(106, 196, 126);
            g_palette.warning          = rgba(214, 175, 90);
            g_palette.danger           = rgba(220, 95, 95);

            g_palette.grid             = rgba(60, 68, 86, 0.85f);
            g_palette.gridSubtle       = rgba(40, 46, 60, 0.55f);
            g_palette.trajectory       = rgba(120, 196, 240);
        }

        void buildLightPalette()
        {
            g_palette.background       = rgba(244, 246, 250);
            g_palette.surface          = rgba(255, 255, 255);
            g_palette.surfaceElevated  = rgba(248, 250, 254);
            g_palette.border           = rgba(214, 220, 230);
            g_palette.divider          = rgba(228, 232, 240);

            g_palette.textPrimary      = rgba(28, 32, 40);
            g_palette.textSecondary    = rgba(80, 88, 104);
            g_palette.textMuted        = rgba(140, 148, 162);

            g_palette.accent           = rgba(36, 116, 200);
            g_palette.accentHover      = rgba(58, 138, 220);
            g_palette.accentActive     = rgba(20, 90, 168);

            g_palette.success          = rgba(46, 160, 80);
            g_palette.warning          = rgba(196, 140, 30);
            g_palette.danger           = rgba(196, 60, 60);

            g_palette.grid             = rgba(180, 190, 210, 0.9f);
            g_palette.gridSubtle       = rgba(210, 218, 232, 0.7f);
            g_palette.trajectory       = rgba(36, 116, 200);
        }

        void writeStyle(float cornerRadius, float windowOpacity)
        {
            ImGuiStyle& style = ImGui::GetStyle();

            style.WindowRounding    = cornerRadius;
            style.ChildRounding     = cornerRadius;
            style.FrameRounding     = cornerRadius * 0.75f;
            style.PopupRounding     = cornerRadius;
            style.ScrollbarRounding = cornerRadius;
            style.GrabRounding      = cornerRadius * 0.75f;
            style.TabRounding       = cornerRadius * 0.6f;

            style.WindowPadding     = ImVec2(14, 14);
            style.FramePadding      = ImVec2(10, 6);
            style.ItemSpacing       = ImVec2(8, 8);
            style.ItemInnerSpacing  = ImVec2(6, 6);
            style.CellPadding       = ImVec2(8, 6);
            style.IndentSpacing     = 18.0f;
            style.ScrollbarSize     = 12.0f;
            style.GrabMinSize       = 10.0f;

            style.WindowBorderSize  = 1.0f;
            style.ChildBorderSize   = 1.0f;
            style.FrameBorderSize   = 1.0f;
            style.PopupBorderSize   = 1.0f;
            style.TabBorderSize     = 0.0f;

            style.WindowTitleAlign   = ImVec2(0.0f, 0.5f);
            style.SelectableTextAlign = ImVec2(0.0f, 0.5f);
            style.ButtonTextAlign    = ImVec2(0.5f, 0.5f);
            style.AntiAliasedLines   = true;
            style.AntiAliasedFill    = true;

            ImVec4* c = style.Colors;
            const auto& p = g_palette;

            ImVec4 bg = p.background; bg.w = windowOpacity;

            c[ImGuiCol_WindowBg]            = bg;
            c[ImGuiCol_ChildBg]             = ImVec4(0, 0, 0, 0);
            c[ImGuiCol_PopupBg]             = ImVec4(p.surfaceElevated.x, p.surfaceElevated.y, p.surfaceElevated.z, 0.98f);
            c[ImGuiCol_Border]              = p.border;
            c[ImGuiCol_BorderShadow]        = ImVec4(0, 0, 0, 0);

            c[ImGuiCol_Text]                = p.textPrimary;
            c[ImGuiCol_TextDisabled]        = p.textMuted;

            // 输入框 / 下拉 / 滑条槽底色：亮色用更亮于背景的浅灰，深色用更暗于背景的深底。
            const ImVec4 frameBg       = g_isLight ? rgba(232, 236, 244)
                                                   : rgba(11, 13, 18);
            const ImVec4 frameBgHover  = g_isLight ? rgba(220, 226, 238)
                                                   : rgba(18, 21, 28);
            const ImVec4 frameBgActive = g_isLight ? rgba(208, 216, 232)
                                                   : rgba(22, 26, 34);
            c[ImGuiCol_FrameBg]             = frameBg;
            c[ImGuiCol_FrameBgHovered]      = frameBgHover;
            c[ImGuiCol_FrameBgActive]       = frameBgActive;

            c[ImGuiCol_TitleBg]             = p.background;
            c[ImGuiCol_TitleBgActive]       = p.background;
            c[ImGuiCol_TitleBgCollapsed]    = p.background;

            c[ImGuiCol_MenuBarBg]           = p.surface;
            c[ImGuiCol_ScrollbarBg]         = ImVec4(0, 0, 0, 0);
            c[ImGuiCol_ScrollbarGrab]       = p.border;
            c[ImGuiCol_ScrollbarGrabHovered]= p.divider;
            c[ImGuiCol_ScrollbarGrabActive] = p.accent;

            c[ImGuiCol_CheckMark]           = p.accent;
            c[ImGuiCol_SliderGrab]          = p.accent;
            c[ImGuiCol_SliderGrabActive]    = p.accentActive;

            // 按钮：在亮色模式下需要与 surface（白）拉开层次。
            const ImVec4 btn       = g_isLight ? rgba(232, 238, 248) : p.surface;
            const ImVec4 btnHover  = g_isLight ? rgba(218, 228, 244) : p.surfaceElevated;
            c[ImGuiCol_Button]              = btn;
            c[ImGuiCol_ButtonHovered]       = btnHover;
            c[ImGuiCol_ButtonActive]        = p.accent;

            c[ImGuiCol_Header]              = ImVec4(p.accent.x, p.accent.y, p.accent.z, 0.18f);
            c[ImGuiCol_HeaderHovered]       = ImVec4(p.accent.x, p.accent.y, p.accent.z, 0.28f);
            c[ImGuiCol_HeaderActive]        = ImVec4(p.accent.x, p.accent.y, p.accent.z, 0.38f);

            c[ImGuiCol_Separator]           = p.divider;
            c[ImGuiCol_SeparatorHovered]    = p.border;
            c[ImGuiCol_SeparatorActive]     = p.accent;

            c[ImGuiCol_ResizeGrip]          = ImVec4(p.accent.x, p.accent.y, p.accent.z, 0.0f);
            c[ImGuiCol_ResizeGripHovered]   = ImVec4(p.accent.x, p.accent.y, p.accent.z, 0.4f);
            c[ImGuiCol_ResizeGripActive]    = ImVec4(p.accent.x, p.accent.y, p.accent.z, 0.65f);

            c[ImGuiCol_Tab]                 = p.surface;
            c[ImGuiCol_TabHovered]          = p.surfaceElevated;
            c[ImGuiCol_TabActive]           = p.surfaceElevated;
            c[ImGuiCol_TabUnfocused]        = p.background;
            c[ImGuiCol_TabUnfocusedActive]  = p.surface;

            c[ImGuiCol_DockingPreview]      = ImVec4(p.accent.x, p.accent.y, p.accent.z, 0.4f);
            c[ImGuiCol_DockingEmptyBg]      = p.background;

            c[ImGuiCol_TableHeaderBg]       = p.surfaceElevated;
            c[ImGuiCol_TableBorderStrong]   = p.border;
            c[ImGuiCol_TableBorderLight]    = p.divider;
            c[ImGuiCol_TableRowBg]          = ImVec4(0, 0, 0, 0);
            c[ImGuiCol_TableRowBgAlt]       = ImVec4(p.surface.x, p.surface.y, p.surface.z, 0.5f);

            c[ImGuiCol_PlotLines]           = p.accent;
            c[ImGuiCol_PlotLinesHovered]    = p.accentHover;
            c[ImGuiCol_PlotHistogram]       = p.accent;
            c[ImGuiCol_PlotHistogramHovered]= p.accentHover;

            c[ImGuiCol_TextSelectedBg]      = ImVec4(p.accent.x, p.accent.y, p.accent.z, 0.35f);
            c[ImGuiCol_NavHighlight]        = p.accent;
            c[ImGuiCol_NavWindowingHighlight] = ImVec4(1, 1, 1, 0.7f);
            c[ImGuiCol_NavWindowingDimBg]   = ImVec4(0, 0, 0, 0.4f);
            c[ImGuiCol_ModalWindowDimBg]    = ImVec4(0, 0, 0, 0.55f);
        }
    }

    void applyDarkTheme(float cornerRadius, float windowOpacity)
    {
        g_isLight = false;
        buildDarkPalette();
        writeStyle(cornerRadius, windowOpacity);
    }

    void applyLightTheme(float cornerRadius, float windowOpacity)
    {
        g_isLight = true;
        buildLightPalette();
        writeStyle(cornerRadius, windowOpacity);
    }

    const ThemePalette& currentPalette()
    {
        return g_palette;
    }
}
