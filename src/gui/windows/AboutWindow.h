#ifndef PEARLCALCULATOR_GUI_ABOUT_WINDOW_H
#define PEARLCALCULATOR_GUI_ABOUT_WINDOW_H

namespace pearl::gui
{
    class AppContext;

    class AboutWindow
    {
    public:
        explicit AboutWindow(AppContext& ctx) : m_ctx(ctx) {}
        void render(bool* open);

    private:
        AppContext& m_ctx;
    };
}

#endif
