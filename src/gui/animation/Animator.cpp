#include "Animator.h"

#include <algorithm>

#include "Easing.h"

namespace pearl::gui
{
    namespace
    {
        double applyEasing(Easing e, double t)
        {
            switch (e)
            {
                case Easing::OutExpo:    return easing::easeOutExpo(t);
                case Easing::OutBack:    return easing::easeOutBack(t);
                case Easing::InOutCubic: return easing::easeInOutCubic(t);
                case Easing::OutCubic:   return easing::easeOutCubic(t);
                case Easing::Linear:
                default:                 return easing::linear(t);
            }
        }
    }

    void Tween::setTarget(double target, double durationSeconds, Easing easing)
    {
        if (durationSeconds <= 0.0)
        {
            snap(target);
            return;
        }
        m_start = m_current;
        m_target = target;
        m_duration = durationSeconds;
        m_elapsed = 0.0;
        m_easing = easing;
    }

    void Tween::snap(double value)
    {
        m_start = value;
        m_current = value;
        m_target = value;
        m_duration = 0.0;
        m_elapsed = 0.0;
    }

    void Tween::update(double deltaTime)
    {
        if (m_elapsed >= m_duration)
        {
            m_current = m_target;
            return;
        }
        m_elapsed = std::min(m_duration, m_elapsed + deltaTime);
        const double t = m_duration > 0.0 ? m_elapsed / m_duration : 1.0;
        const double eased = applyEasing(m_easing, t);
        m_current = m_start + (m_target - m_start) * eased;
    }

    void Animator::update(double deltaTime)
    {
        const double dt = deltaTime * std::max(0.0, m_speed);
        for (auto& [_, tween] : m_tweens)
        {
            tween.update(dt);
        }
    }

    Tween& Animator::tween(const std::string& key, double initial)
    {
        auto [it, inserted] = m_tweens.try_emplace(key);
        if (inserted)
        {
            it->second.snap(initial);
        }
        return it->second;
    }

    double Animator::driveTo(const std::string& key,
                             double target,
                             double durationSeconds,
                             Easing easing,
                             double initialIfNew)
    {
        Tween& tw = tween(key, initialIfNew);
        if (tw.target() != target)
        {
            tw.setTarget(target, durationSeconds, easing);
        }
        return tw.current();
    }
}
