#ifndef PEARLCALCULATOR_GUI_ANIMATOR_H
#define PEARLCALCULATOR_GUI_ANIMATOR_H

#include <string>
#include <unordered_map>

namespace pearl::gui
{
    enum class Easing
    {
        Linear,
        OutExpo,
        OutBack,
        InOutCubic,
        OutCubic
    };

    // 单值缓动插值器：每帧调用 update(deltaTime) 后读 current()。
    class Tween
    {
    public:
        void setTarget(double target, double durationSeconds, Easing easing = Easing::OutExpo);
        void snap(double value);
        void update(double deltaTime);

        [[nodiscard]] double current() const { return m_current; }
        [[nodiscard]] double target() const { return m_target; }
        [[nodiscard]] bool isAnimating() const { return m_elapsed < m_duration; }

    private:
        double m_start{};
        double m_current{};
        double m_target{};
        double m_duration{};
        double m_elapsed{};
        Easing m_easing{Easing::OutExpo};
    };

    // 命名插值表：按 key 动态创建/查询；用于 hover、underline、宽高过渡等。
    class Animator
    {
    public:
        void setSpeed(double scale) { m_speed = scale; }
        [[nodiscard]] double speed() const { return m_speed; }

        void update(double deltaTime);

        // 取得（或创建）一个命名 Tween，初值为 initial。
        Tween& tween(const std::string& key, double initial = 0.0);

        // 将 key 的 Tween 缓动到 target。
        double driveTo(const std::string& key,
                       double target,
                       double durationSeconds,
                       Easing easing = Easing::OutExpo,
                       double initialIfNew = 0.0);

    private:
        std::unordered_map<std::string, Tween> m_tweens;
        double m_speed{1.0};
    };
}

#endif
