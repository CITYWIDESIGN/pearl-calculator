#ifndef PEARLCALCULATOR_GUI_WIDGETS_TOAST_H
#define PEARLCALCULATOR_GUI_WIDGETS_TOAST_H

#include <deque>
#include <string>

#include <imgui.h>

namespace pearl::gui::widgets
{
    enum class ToastKind { Info, Success, Warning, Error };

    struct Toast
    {
        std::string text;
        ToastKind kind = ToastKind::Info;
        double remainingSeconds = 3.5;
        double age = 0.0;
    };

    class ToastQueue
    {
    public:
        void push(std::string text, ToastKind kind = ToastKind::Info, double seconds = 3.5);
        void render(double deltaTime);

    private:
        std::deque<Toast> m_toasts;
    };
}

#endif
