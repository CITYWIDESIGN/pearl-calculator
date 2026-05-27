#ifndef PEARLCALCULATOR_GUI_APP_CONTEXT_H
#define PEARLCALCULATOR_GUI_APP_CONTEXT_H

#include <chrono>
#include <deque>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "../animation/Animator.h"
#include "../fonts/FontManager.h"
#include "../settings/SettingsStore.h"
#include "../widgets/Toast.h"
#include "../../data.h"

namespace pearl::gui
{
    // GUI 表单输入态（与 g_appState 解耦）。
    struct CalculatorForm
    {
        std::string targetX = "0";
        std::string targetY = "128";
        std::string targetZ = "0";
        std::string redTnt  = "0";
        std::string blueTnt = "0";
        std::string direction = "North";
        bool useY = false;
    };

    // 控制台日志。
    struct ConsoleEntry
    {
        std::string time;
        std::string text;
        widgets::ToastKind kind = widgets::ToastKind::Info;
    };

    enum class RightPanelMode
    {
        Plans = 0,
        Trajectory = 1
    };

    class AppContext
    {
    public:
        AppContext();
        ~AppContext();

        void initialize();
        void update(double deltaTime);

        void runSolve();
        void runSimulationFromForm();
        void selectPlan(int index);
        void resetForm();
        void emitLog(const std::string& text, widgets::ToastKind kind = widgets::ToastKind::Info);

        void applyMenuSettings();

        SettingsStore&             settings()  { return m_settings; }
        const SettingsStore&       settings() const { return m_settings; }
        FontManager&               fonts()     { return m_fonts; }
        Animator&                  animator()  { return m_animator; }
        widgets::ToastQueue&       toasts()    { return m_toasts; }

        CalculatorForm&            form()      { return m_form; }
        std::vector<LaunchPlan>&   plans()     { return m_plans; }
        std::vector<SimulationPoint>& trajectory() { return m_trajectory; }
        std::optional<int>&        selectedPlanIndex() { return m_selectedPlanIndex; }
        std::optional<int>&        selectedTrajIndex() { return m_selectedTrajIndex; }
        RightPanelMode&            rightPanel() { return m_rightPanel; }

        const std::deque<ConsoleEntry>& consoleEntries() const { return m_console; }

        // 列表入场动效用：返回距离最近一次结果生成 / 轨迹生成的秒数。
        double secondsSincePlansGen() const;
        double secondsSinceTrajGen() const;
        // 重置入场动画时间戳（例如切换 Tab 时重新播放）
        void resetPlansGenTimer();
        void resetTrajGenTimer();

        const std::string& statusMessage() const { return m_statusMessage; }
        void setStatusMessage(std::string s) { m_statusMessage = std::move(s); }

        const std::string& settingsPath() const { return m_settingsPath; }

    private:
        std::string m_settingsPath;
        SettingsStore m_settings;
        FontManager m_fonts;
        Animator m_animator;
        widgets::ToastQueue m_toasts;

        CalculatorForm m_form;
        std::vector<LaunchPlan> m_plans;
        std::vector<SimulationPoint> m_trajectory;
        std::optional<int> m_selectedPlanIndex;
        std::optional<int> m_selectedTrajIndex;
        RightPanelMode m_rightPanel = RightPanelMode::Plans;

        std::chrono::steady_clock::time_point m_plansGenStart{};
        std::chrono::steady_clock::time_point m_trajGenStart{};

        std::deque<ConsoleEntry> m_console;
        std::string m_statusMessage = "准备就绪。";
    };
}

#endif
