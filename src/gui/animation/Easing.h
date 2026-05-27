#ifndef PEARLCALCULATOR_GUI_EASING_H
#define PEARLCALCULATOR_GUI_EASING_H

namespace pearl::gui::easing
{
    double linear(double t);
    double easeOutExpo(double t);
    double easeOutBack(double t);
    double easeInOutCubic(double t);
    double easeOutCubic(double t);
}

#endif
