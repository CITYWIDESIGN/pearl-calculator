#ifndef PEARLCALCULATOR_GUI_APPLICATION_H
#define PEARLCALCULATOR_GUI_APPLICATION_H

#include <memory>

namespace pearl::gui
{
    class AppContext;
    class CalculatorWindow;
    class SettingsWindow;
    class ViewportWindow;
    class ConsoleWindow;
    class AboutWindow;

    // 顶层 GUI 应用：负责 GLFW 窗口、ImGui 后端、Docking 布局、主循环。
    class Application
    {
    public:
        Application();
        ~Application();

        int run();

    private:
        bool initWindow();
        void initImGui();
        void shutdownImGui();
        void shutdownWindow();
        void buildDefaultDockLayout(unsigned int dockspaceId);
        void renderMenuBar();
        void renderDockspace();
        void renderStatusBar();
        float renderTitleBar();

        struct Impl;
        std::unique_ptr<Impl> m_impl;
    };
}

#endif
