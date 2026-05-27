#include "Easing.h"

#include <algorithm>
#include <cmath>

namespace pearl::gui::easing
{
    double linear(double t)
    {
        return std::clamp(t, 0.0, 1.0);
    }

    double easeOutExpo(double t)
    {
        t = std::clamp(t, 0.0, 1.0);
        if (t >= 1.0) return 1.0;
        return 1.0 - std::pow(2.0, -10.0 * t);
    }

    double easeOutBack(double t)
    {
        t = std::clamp(t, 0.0, 1.0);
        constexpr double c1 = 1.70158;
        constexpr double c3 = c1 + 1.0;
        const double x = t - 1.0;
        return 1.0 + c3 * x * x * x + c1 * x * x;
    }

    double easeInOutCubic(double t)
    {
        t = std::clamp(t, 0.0, 1.0);
        if (t < 0.5) return 4.0 * t * t * t;
        const double f = -2.0 * t + 2.0;
        return 1.0 - f * f * f / 2.0;
    }

    double easeOutCubic(double t)
    {
        t = std::clamp(t, 0.0, 1.0);
        const double x = 1.0 - t;
        return 1.0 - x * x * x;
    }
}
