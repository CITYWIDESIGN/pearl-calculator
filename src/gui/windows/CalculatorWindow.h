#ifndef PEARLCALCULATOR_GUI_CALCULATOR_WINDOW_H
#define PEARLCALCULATOR_GUI_CALCULATOR_WINDOW_H

namespace pearl::gui
{
    class AppContext;

    class CalculatorWindow
    {
    public:
        explicit CalculatorWindow(AppContext& ctx) : m_ctx(ctx) {}
        void render(bool* open);

    private:
        AppContext& m_ctx;
        int m_activeTab = 0;
    };
}

#endif
