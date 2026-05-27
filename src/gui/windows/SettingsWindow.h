#ifndef PEARLCALCULATOR_GUI_SETTINGS_WINDOW_H
#define PEARLCALCULATOR_GUI_SETTINGS_WINDOW_H

namespace pearl::gui
{
    class AppContext;

    class SettingsWindow
    {
    public:
        explicit SettingsWindow(AppContext& ctx) : m_ctx(ctx) {}
        void render(bool* open);

    private:
        AppContext& m_ctx;
        int m_activeTab = 0;
    };
}

#endif
