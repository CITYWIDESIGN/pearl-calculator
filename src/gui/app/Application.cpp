#include "Application.h"

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <string>

#ifdef _WIN32
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
  #include <commdlg.h>
  #include <GLFW/glfw3.h>
  #define GLFW_EXPOSE_NATIVE_WIN32
  #include <GLFW/glfw3native.h>
#endif

#include <imgui.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#ifndef _WIN32
  #include <GLFW/glfw3.h>
#endif

#include "AppContext.h"
#include "../theme/Theme.h"
#include "../i18n/I18n.h"
#include "../widgets/Card.h"
#include "../widgets/Toast.h"
#include "../windows/AboutWindow.h"
#include "../windows/CalculatorWindow.h"
#include "../windows/ConsoleWindow.h"
#include "../windows/SettingsWindow.h"
#include "../../data.h"
#include "../../utils/loadConfig.h"

namespace pearl::gui
{
    struct Application::Impl
    {
        GLFWwindow* window = nullptr;
        AppContext ctx;

        std::unique_ptr<CalculatorWindow> calcWin;
        std::unique_ptr<SettingsWindow>   settingsWin;
        std::unique_ptr<ConsoleWindow>    consoleWin;
        std::unique_ptr<AboutWindow>      aboutWin;

        bool showSettings = false;
        bool showConsole = false;
        bool showAbout = false;

        std::chrono::steady_clock::time_point lastTime;
        std::string iniPath;
    };

    Application::Application() : m_impl(std::make_unique<Impl>()) {}
    Application::~Application() = default;

    namespace
    {
        void glfwErrorCallback(int code, const char* msg)
        {
            std::cerr << "[GLFW] (" << code << ") " << msg << std::endl;
        }

        // 打开"加载配置文件"对话框；返回选中的路径，取消则返回空串。
        std::string openLoadConfigDialog(GLFWwindow* window)
        {
#ifdef _WIN32
            char path[MAX_PATH] = {};
            OPENFILENAMEA ofn{};
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner   = glfwGetWin32Window(window);
            ofn.lpstrFilter = "JSON Config (*.json)\0*.json\0All Files (*.*)\0*.*\0";
            ofn.lpstrFile   = path;
            ofn.nMaxFile    = sizeof(path);
            ofn.lpstrTitle  = "Load Config";
            ofn.Flags       = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
            if (GetOpenFileNameA(&ofn))
                return std::string(path);
#endif
            return {};
        }
    }

    bool Application::initWindow()
    {
        glfwSetErrorCallback(glfwErrorCallback);
        if (!glfwInit()) return false;

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        // 关掉 Windows 原生标题栏，自己画。
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

        constexpr int kInitW = 1200;
        constexpr int kInitH = 900;
        m_impl->window = glfwCreateWindow(kInitW, kInitH, "Pearl Calculator", nullptr, nullptr);
        if (!m_impl->window) { glfwTerminate(); return false; }

        // 居中到主显示器。
        if (GLFWmonitor* mon = glfwGetPrimaryMonitor())
        {
            int mx = 0, my = 0, mw = 0, mh = 0;
            glfwGetMonitorWorkarea(mon, &mx, &my, &mw, &mh);
            if (mw > 0 && mh > 0)
            {
                glfwSetWindowPos(m_impl->window,
                                 mx + (mw - kInitW) / 2,
                                 my + (mh - kInitH) / 2);
            }
        }

        glfwMakeContextCurrent(m_impl->window);
        glfwSwapInterval(1);
        glfwShowWindow(m_impl->window);
        return true;
    }

    void Application::initImGui()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        // 不启用 Docking：避免子窗口被吸附 / 合并到主窗口。
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        std::error_code ec;
        std::filesystem::create_directories("../config", ec);
        m_impl->iniPath = "../config/imgui_layout.ini";
        io.IniFilename = m_impl->iniPath.c_str();

        ImGui_ImplGlfw_InitForOpenGL(m_impl->window, true);
        ImGui_ImplOpenGL3_Init("#version 330");

        // 多视口模式下：让子视口窗口与主窗口风格一致（无系统标题栏）。
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGuiStyle& style = ImGui::GetStyle();
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        const auto& menu = m_impl->ctx.settings().menu();
        m_impl->ctx.fonts().load(menu.uiFont, menu.monoFont,
                                 {14.0f, 16.0f, 20.0f},
                                 menu.fontSize);

        m_impl->ctx.applyMenuSettings();
    }

    void Application::shutdownImGui()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void Application::shutdownWindow()
    {
        if (m_impl->window) glfwDestroyWindow(m_impl->window);
        glfwTerminate();
    }

    void Application::buildDefaultDockLayout(unsigned int /*dockspaceId*/) {}

    namespace
    {
        struct DragState {
            bool dragging = false;
            int  startMouseX = 0, startMouseY = 0;
            int  startWinX = 0,   startWinY = 0;
        };
        DragState g_drag;

        // 标题栏自绘按钮的 hit-test 状态（key -> 按下中）。
        bool g_btnPressed[3] = {false, false, false};

        // 右下角缩放手柄状态。
        struct ResizeState {
            bool resizing = false;
            int  startMouseX = 0, startMouseY = 0;
            int  startW = 0, startH = 0;
        };
        ResizeState g_resize;
    }

    void Application::renderMenuBar()
    {
        // 不再使用 BeginMainMenuBar：标题栏自绘。
    }

    namespace
    {
        // 在右下角绘制并处理缩放手柄。返回值仅用于让标题栏拖拽逻辑互斥。
        bool renderSizeGripImpl(GLFWwindow* window, ImGuiViewport* vp,
                                const ThemePalette& palette)
        {
            const float size = 16.0f;
            const ImVec2 corner(vp->Pos.x + vp->Size.x, vp->Pos.y + vp->Size.y);
            const ImVec2 a(corner.x - size, corner.y - size);

            const ImVec2 mp = ImGui::GetIO().MousePos;
            // 只在右下角三角区（mp 落在 x+y >= corner-size 的下三角）激活
            const bool inTriangle = mp.x >= a.x && mp.x <= corner.x
                                 && mp.y >= a.y && mp.y <= corner.y
                                 && (corner.x - mp.x) + (corner.y - mp.y) <= size;

            const bool maximized = glfwGetWindowAttrib(window, GLFW_MAXIMIZED) == GLFW_TRUE;
            const bool blockedByImGui = ImGui::IsAnyItemActive();

            const bool hot = inTriangle && !maximized && !blockedByImGui;

            ImDrawList* fdl = ImGui::GetForegroundDrawList();

            // 三条对角短线，间距 4px；颜色随 hover / drag 状态由次到主。
            ImVec4 lineCol = palette.textMuted;
            if (hot)            lineCol = palette.textSecondary;
            if (g_resize.resizing) lineCol = palette.accent;
            const ImU32 col = ImGui::ColorConvertFloat4ToU32(lineCol);

            const float thick = 1.5f;
            for (int i = 0; i < 3; ++i)
            {
                const float off = 4.0f + i * 4.0f;
                fdl->AddLine(ImVec2(corner.x - off, corner.y - 2.0f),
                             ImVec2(corner.x - 2.0f, corner.y - off),
                             col, thick);
            }

            // 鼠标光标提示
            if (hot || g_resize.resizing)
                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNWSE);

            // 开始 / 持续 / 结束拖拽
            if (hot && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                g_resize.resizing = true;
                double mx, my; glfwGetCursorPos(window, &mx, &my);
                int wx, wy; glfwGetWindowPos(window, &wx, &wy);
                g_resize.startMouseX = wx + static_cast<int>(mx);
                g_resize.startMouseY = wy + static_cast<int>(my);
                int ww, wh; glfwGetWindowSize(window, &ww, &wh);
                g_resize.startW = ww;
                g_resize.startH = wh;
            }
            if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) g_resize.resizing = false;
            if (g_resize.resizing)
            {
                double mx, my; glfwGetCursorPos(window, &mx, &my);
                int wx, wy; glfwGetWindowPos(window, &wx, &wy);
                const int globalX = wx + static_cast<int>(mx);
                const int globalY = wy + static_cast<int>(my);
                const int newW = std::max(640, g_resize.startW + (globalX - g_resize.startMouseX));
                const int newH = std::max(420, g_resize.startH + (globalY - g_resize.startMouseY));
                glfwSetWindowSize(window, newW, newH);
            }

            return hot || g_resize.resizing;
        }
    }

    float Application::renderTitleBar()
    {
        const auto& palette = currentPalette();
        const float h = 36.0f;
        const float btnW = 44.0f; // 三个控制按钮统一宽度
        const float btnH = h;     // 与标题栏等高
        const float controlsW = btnW * 3.0f;

        ImGuiViewport* vp = ImGui::GetMainViewport();

        // ===== 标题栏底（菜单条） =====
        ImGui::SetNextWindowPos(vp->Pos);
        ImGui::SetNextWindowSize(ImVec2(vp->Size.x - controlsW, h));

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_WindowBg, palette.surface);
        ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(0, 0, 0, 0));

        const ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar
                                     | ImGuiWindowFlags_NoResize
                                     | ImGuiWindowFlags_NoMove
                                     | ImGuiWindowFlags_NoScrollbar
                                     | ImGuiWindowFlags_NoSavedSettings
                                     | ImGuiWindowFlags_NoCollapse
                                     | ImGuiWindowFlags_NoBringToFrontOnFocus
                                     | ImGuiWindowFlags_MenuBar;

        ImGui::Begin("##title_bar", nullptr, flags);

        // 一级菜单按钮：水平 padding=14，菜单项之间额外 6px 呼吸感
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,
                            ImVec2(14.0f, (h - ImGui::GetFontSize()) * 0.5f));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(6.0f, 0.0f));

        // 二级（弹出）菜单：恢复合理的 padding/spacing，并保证最小宽度。
        // 这些 push 会被 EndMenuBar 之后弹出窗口继承，因此必须在 BeginMenuBar 前压栈。
        ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 6.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, 1.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(6.0f, 6.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,   ImVec2(8.0f, 4.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(6.0f, 4.0f));

        auto menuItem = [](const char* label, const char* shortcut, bool* selected = nullptr)
        {
            // 用宽度配合 ImGui::MenuItem 自身的左对齐 label / 右对齐 shortcut。
            // 给最小宽度（含 shortcut 显示空间）。
            return selected
                ? ImGui::MenuItem(label, shortcut, selected)
                : ImGui::MenuItem(label, shortcut);
        };

        float menuRight = vp->Pos.x;
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu(t("menu.file")))
            {
                ImGui::SetWindowSize(ImVec2(240, 0));
                if (menuItem(t("menu.load_config"), nullptr))
                {
                    const std::string p = openLoadConfigDialog(m_impl->window);
                    if (!p.empty())
                    {
                        try
                        {
                            loadConfiguration(p);
                            m_impl->ctx.emitLog(t("toast.config_loaded"),
                                                widgets::ToastKind::Success);
                        }
                        catch (const std::exception& ex)
                        {
                            m_impl->ctx.emitLog(std::string("Load failed: ") + ex.what(),
                                                widgets::ToastKind::Error);
                        }
                    }
                    else
                    {
                        m_impl->ctx.emitLog(t("toast.config_cancel"),
                                            widgets::ToastKind::Info);
                    }
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu(t("menu.view")))
            {
                ImGui::SetWindowSize(ImVec2(220, 0));
                ImGui::MenuItem(t("menu.settings"), nullptr, &m_impl->showSettings);
                ImGui::MenuItem(t("menu.console"),  nullptr, &m_impl->showConsole);
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu(t("menu.help")))
            {
                ImGui::SetWindowSize(ImVec2(220, 0));
                ImGui::MenuItem(t("menu.about"), nullptr, &m_impl->showAbout);
                ImGui::EndMenu();
            }
            menuRight = ImGui::GetCursorScreenPos().x;
            ImGui::EndMenuBar();
        }
        ImGui::PopStyleVar(7); // FramePadding + ItemSpacing + PopupRounding + PopupBorderSize + WindowPadding + ItemSpacing + ItemInnerSpacing

        ImGui::End();
        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar(3);

        // ===== 标题文本（居中）+ 控制按钮（自绘几何图形）=====
        ImDrawList* fdl = ImGui::GetForegroundDrawList();

        const char* title = "Pearl Calculator";
        const ImVec2 ts = ImGui::CalcTextSize(title);
        fdl->AddText(ImVec2(vp->Pos.x + (vp->Size.x - ts.x) * 0.5f,
                            vp->Pos.y + (h - ts.y) * 0.5f),
                     ImGui::ColorConvertFloat4ToU32(palette.textMuted),
                     title);

        // 标题栏背景延伸到右上角（菜单条窗口宽度只到 vp.w-controlsW）
        fdl->AddRectFilled(
            ImVec2(vp->Pos.x + vp->Size.x - controlsW, vp->Pos.y),
            ImVec2(vp->Pos.x + vp->Size.x,             vp->Pos.y + h),
            ImGui::ColorConvertFloat4ToU32(palette.surface));

        // 三个控制按钮：完全用几何线段绘制，不依赖任何字体字形，避免贴图缺失。
        const ImVec2 mp = ImGui::GetIO().MousePos;
        const bool maximized = glfwGetWindowAttrib(m_impl->window, GLFW_MAXIMIZED) == GLFW_TRUE;

        const float baseX = vp->Pos.x + vp->Size.x - controlsW;
        const float baseY = vp->Pos.y;

        const bool blockedByImGui = ImGui::IsAnyItemActive()
                                 || ImGui::IsPopupOpen("", ImGuiPopupFlags_AnyPopupId);

        auto drawIcon = [&](int kind, const ImVec2& center, ImU32 col, float scale = 1.0f)
        {
            const float s = 5.0f * scale;       // 图标半边长（按 press 状态缩放）
            const float thickness = 1.6f;
            switch (kind)
            {
                case 0: // 最小化：一条短横（垂直居中）
                {
                    fdl->AddLine(ImVec2(center.x - s, center.y),
                                 ImVec2(center.x + s, center.y),
                                 col, thickness);
                    break;
                }
                case 1: // 最大化 / 还原
                {
                    if (maximized)
                    {
                        // 还原图标：前后两层方框
                        const float off = 1.6f * scale;
                        // 后层：右上角
                        fdl->AddRect(ImVec2(center.x - s + off, center.y - s - off),
                                     ImVec2(center.x + s + off, center.y + s - off),
                                     col, 0.0f, 0, thickness);
                        // 前层：左下角（先用底色擦后层一截，再画前层）
                        fdl->AddRectFilled(ImVec2(center.x - s - off, center.y - s + off),
                                           ImVec2(center.x + s - off, center.y + s + off),
                                           ImGui::ColorConvertFloat4ToU32(palette.surface));
                        fdl->AddRect(ImVec2(center.x - s - off, center.y - s + off),
                                     ImVec2(center.x + s - off, center.y + s + off),
                                     col, 0.0f, 0, thickness);
                    }
                    else
                    {
                        // 最大化：单方框
                        fdl->AddRect(ImVec2(center.x - s, center.y - s),
                                     ImVec2(center.x + s, center.y + s),
                                     col, 0.0f, 0, thickness);
                    }
                    break;
                }
                case 2: // 关闭：两条对角线
                {
                    fdl->AddLine(ImVec2(center.x - s, center.y - s),
                                 ImVec2(center.x + s, center.y + s),
                                 col, thickness);
                    fdl->AddLine(ImVec2(center.x + s, center.y - s),
                                 ImVec2(center.x - s, center.y + s),
                                 col, thickness);
                    break;
                }
                default: break;
            }
        };

        const ImVec4 hoverNeutral = palette.surfaceElevated;
        const ImVec4 hoverDanger  = ImVec4(0.85f, 0.30f, 0.30f, 1.0f);

        Animator& anim = m_impl->ctx.animator();

        for (int i = 0; i < 3; ++i)
        {
            const ImVec2 a(baseX + btnW * i, baseY);
            const ImVec2 b(a.x + btnW, a.y + btnH);
            const bool hovered = !blockedByImGui
                              && mp.x >= a.x && mp.x < b.x
                              && mp.y >= a.y && mp.y < b.y;

            // hover 强度（0..1）— OutCubic ~150ms 渐入渐出
            const std::string hoverKey = "titlebtn_hover_" + std::to_string(i);
            const float hoverT = static_cast<float>(anim.driveTo(
                hoverKey, hovered ? 1.0 : 0.0, 0.16, Easing::OutCubic));
            // press 缩放：按下时图标缩到 0.92，松开回到 1.0
            const std::string pressKey = "titlebtn_press_" + std::to_string(i);
            const float pressT = static_cast<float>(anim.driveTo(
                pressKey, g_btnPressed[i] ? 1.0 : 0.0, 0.10, Easing::OutCubic));

            if (hoverT > 0.001f)
            {
                ImVec4 bg = (i == 2) ? hoverDanger : hoverNeutral;
                bg.w *= hoverT;
                fdl->AddRectFilled(a, b, ImGui::ColorConvertFloat4ToU32(bg));
            }

            // 图标颜色：关闭按钮 hover 程度越大越白；其它按钮保持 textPrimary
            ImVec4 fg = palette.textPrimary;
            if (i == 2)
            {
                fg.x = palette.textPrimary.x + (1.0f - palette.textPrimary.x) * hoverT;
                fg.y = palette.textPrimary.y + (1.0f - palette.textPrimary.y) * hoverT;
                fg.z = palette.textPrimary.z + (1.0f - palette.textPrimary.z) * hoverT;
            }
            const ImVec2 center((a.x + b.x) * 0.5f, (a.y + b.y) * 0.5f);

            // press 时图标整体缩 8%
            const float scale = 1.0f - pressT * 0.08f;
            drawIcon(i, center, ImGui::ColorConvertFloat4ToU32(fg), scale);

            // 点击：down→记录按下；up 时若仍 hover 则触发。
            if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                g_btnPressed[i] = true;
            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
            {
                if (g_btnPressed[i] && hovered)
                {
                    if (i == 0) glfwIconifyWindow(m_impl->window);
                    else if (i == 1)
                    {
                        if (maximized) glfwRestoreWindow(m_impl->window);
                        else           glfwMaximizeWindow(m_impl->window);
                    }
                    else        glfwSetWindowShouldClose(m_impl->window, GLFW_TRUE);
                }
                g_btnPressed[i] = false;
            }
        }

        // 底部细分割线
        fdl->AddLine(ImVec2(vp->Pos.x, vp->Pos.y + h - 1),
                     ImVec2(vp->Pos.x + vp->Size.x, vp->Pos.y + h - 1),
                     ImGui::ColorConvertFloat4ToU32(palette.border), 1.0f);

        // ===== 拖拽区（菜单右侧 ~ 控制按钮左侧空白）=====
        const bool overControls = mp.x >= baseX && mp.x < baseX + controlsW
                               && mp.y >= baseY && mp.y < baseY + h;
        const ImVec2 dragMin(menuRight + 4.0f, vp->Pos.y);
        const ImVec2 dragMax(baseX, vp->Pos.y + h);
        const bool overTitle = !overControls
                            && mp.x >= dragMin.x && mp.x <= dragMax.x
                            && mp.y >= dragMin.y && mp.y <= dragMax.y
                            && !ImGui::IsAnyItemHovered()
                            && !ImGui::IsAnyItemActive();

        if (overTitle && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            g_drag.dragging = true;
            double mx, my; glfwGetCursorPos(m_impl->window, &mx, &my);
            int wx, wy; glfwGetWindowPos(m_impl->window, &wx, &wy);
            g_drag.startMouseX = wx + static_cast<int>(mx);
            g_drag.startMouseY = wy + static_cast<int>(my);
            g_drag.startWinX = wx;
            g_drag.startWinY = wy;
        }
        if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) g_drag.dragging = false;
        if (g_drag.dragging && !maximized)
        {
            double mx, my; glfwGetCursorPos(m_impl->window, &mx, &my);
            int wx, wy; glfwGetWindowPos(m_impl->window, &wx, &wy);
            const int globalX = wx + static_cast<int>(mx);
            const int globalY = wy + static_cast<int>(my);
            glfwSetWindowPos(m_impl->window,
                             g_drag.startWinX + (globalX - g_drag.startMouseX),
                             g_drag.startWinY + (globalY - g_drag.startMouseY));
        }
        if (overTitle && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
        {
            if (maximized) glfwRestoreWindow(m_impl->window);
            else           glfwMaximizeWindow(m_impl->window);
        }

        return h;
    }

    void Application::renderDockspace()
    {
        // 主窗口直接铺满 viewport。不使用 Dockspace —— 子窗口通过菜单切换显示。
    }

    void Application::renderStatusBar() {}

    int Application::run()
    {
        if (!initWindow())
        {
            std::cerr << "[Pearl] GLFW window 创建失败。" << std::endl;
            return 1;
        }
        m_impl->ctx.initialize();
        initImGui();

        m_impl->calcWin     = std::make_unique<CalculatorWindow>(m_impl->ctx);
        m_impl->settingsWin = std::make_unique<SettingsWindow>(m_impl->ctx);
        m_impl->consoleWin  = std::make_unique<ConsoleWindow>(m_impl->ctx);
        m_impl->aboutWin    = std::make_unique<AboutWindow>(m_impl->ctx);

        m_impl->lastTime = std::chrono::steady_clock::now();

        while (!glfwWindowShouldClose(m_impl->window))
        {
            try
            {
                glfwPollEvents();

                const auto now = std::chrono::steady_clock::now();
                const double dt = std::chrono::duration<double>(now - m_impl->lastTime).count();
                m_impl->lastTime = now;

                m_impl->ctx.update(dt);

                ImGui_ImplOpenGL3_NewFrame();
                ImGui_ImplGlfw_NewFrame();
                ImGui::NewFrame();

                const float titleH = renderTitleBar();

                // 计算器主窗口铺满标题栏下方的所有区域。
                m_impl->calcWin->render(nullptr);

                // 右下角缩放手柄（在所有窗口之上；最大化时不响应）
                renderSizeGripImpl(m_impl->window, ImGui::GetMainViewport(), currentPalette());

                // 子窗口默认居中显示，但允许用户自由拖拽（包括拖出主窗口）。
                ImGuiViewport* vp = ImGui::GetMainViewport();
                const ImVec2 workMin(vp->Pos.x, vp->Pos.y + titleH);
                const ImVec2 workMax(vp->Pos.x + vp->Size.x, vp->Pos.y + vp->Size.y);

                // 子窗口出场 / 退场动画：
                //   open  -> alpha 0→1（OutCubic 260ms）+ translateY +20px→0（OutBack 320ms 带轻微回弹）
                //   close -> alpha 1→0（OutCubic 220ms）
                //   alpha < 0.005 时彻底卸载，避免覆盖输入
                // 注意：只在入场动画进行时强制位置；之后让 ImGui 自己维护，用户可自由拖动。
                Animator& subAnim = m_impl->ctx.animator();
                auto renderSubWindow = [&](const char* tweenKey,
                                           bool& open,
                                           const ImVec2& size,
                                           auto&& renderFn)
                {
                    const float a = static_cast<float>(subAnim.driveTo(
                        std::string(tweenKey) + "_a",
                        open ? 1.0 : 0.0,
                        open ? 0.26 : 0.22,
                        Easing::OutCubic));
                    const float p = static_cast<float>(subAnim.driveTo(
                        std::string(tweenKey) + "_p",
                        open ? 1.0 : 0.0,
                        open ? 0.32 : 0.22,
                        open ? Easing::OutBack : Easing::OutCubic));
                    if (a < 0.005f) return;

                    // 仅在入场动画进行中（open == true 且 p < 1）强制位置；
                    // 入场完成后（或处于关闭收尾阶段）不再调用 SetNextWindowPos，
                    // 这样用户拖动出来的位置才能保留。
                    if (open && p < 0.999f)
                    {
                        const ImVec2 center((workMin.x + workMax.x) * 0.5f,
                                            (workMin.y + workMax.y) * 0.5f);
                        const float restY  = center.y - size.y * 0.5f;
                        const float startY = restY + 20.0f;
                        const float curY   = startY + (restY - startY) * p;
                        ImGui::SetNextWindowPos(ImVec2(center.x - size.x * 0.5f, curY),
                                                ImGuiCond_Always);
                    }
                    ImGui::SetNextWindowSize(size, ImGuiCond_Appearing);

                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, a);

                    bool localOpen = true;
                    renderFn(&localOpen);
                    if (!localOpen) open = false;

                    ImGui::PopStyleVar();
                };

                renderSubWindow("subwin_settings", m_impl->showSettings, ImVec2(560, 460),
                    [&](bool* o) { m_impl->settingsWin->render(o); });
                renderSubWindow("subwin_console",  m_impl->showConsole,  ImVec2(680, 360),
                    [&](bool* o) { m_impl->consoleWin->render(o);  });
                renderSubWindow("subwin_about",    m_impl->showAbout,    ImVec2(420, 260),
                    [&](bool* o) { m_impl->aboutWin->render(o);    });

                m_impl->ctx.toasts().render(dt);

                if (ImGui::IsKeyPressed(ImGuiKey_F5, false)) m_impl->ctx.runSolve();
                if (ImGui::IsKeyPressed(ImGuiKey_F6, false)) m_impl->ctx.runSimulationFromForm();
                if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_R, false))
                    m_impl->ctx.resetForm();

                ImGui::Render();
                int dispW, dispH;
                glfwGetFramebufferSize(m_impl->window, &dispW, &dispH);
                glViewport(0, 0, dispW, dispH);
                const auto& bg = currentPalette().background;
                glClearColor(bg.x, bg.y, bg.z, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

                if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
                {
                    GLFWwindow* backupCtx = glfwGetCurrentContext();
                    ImGui::UpdatePlatformWindows();
                    ImGui::RenderPlatformWindowsDefault();
                    glfwMakeContextCurrent(backupCtx);
                }

                glfwSwapBuffers(m_impl->window);
            }
            catch (const std::exception& ex)
            {
                std::cerr << "[Pearl] 帧异常: " << ex.what() << std::endl;
                break;
            }
        }

        if (m_impl->ctx.settings().menu().saveLayout)
        {
            ImGui::SaveIniSettingsToDisk(ImGui::GetIO().IniFilename);
        }
        m_impl->ctx.settings().save();

        shutdownImGui();
        shutdownWindow();
        return 0;
    }
}
