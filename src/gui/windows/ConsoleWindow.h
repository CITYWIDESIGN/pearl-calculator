#ifndef PEARLCALCULATOR_GUI_CONSOLE_WINDOW_H
#define PEARLCALCULATOR_GUI_CONSOLE_WINDOW_H

namespace pearl::gui
{
    class AppContext;

    class ConsoleWindow
    {
    public:
        explicit ConsoleWindow(AppContext& ctx) : m_ctx(ctx) {}
        void render(bool* open);

    private:
        AppContext& m_ctx;
    };
}

#endif
