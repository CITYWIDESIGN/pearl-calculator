#include "AppContext.h"

#include <algorithm>
#include <ctime>
#include <iomanip>
#include <sstream>

#include "../theme/Theme.h"
#include "../i18n/I18n.h"
#include "../../calcTNT/solve.h"
#include "../../simulationPearl/simulatePearl.h"

namespace pearl::gui
{
    namespace
    {
        bool parseDouble(const std::string& s, double& out)
        {
            try
            {
                std::size_t n = 0;
                out = std::stod(s, &n);
                return n == s.size();
            }
            catch (...) { return false; }
        }

        bool parseInt(const std::string& s, int& out)
        {
            try
            {
                std::size_t n = 0;
                out = std::stoi(s, &n);
                return n == s.size();
            }
            catch (...) { return false; }
        }

        bool isValidDirection(const std::string& d)
        {
            return d == "North" || d == "South" || d == "East" || d == "West";
        }

        std::string nowStamp()
        {
            std::time_t t = std::time(nullptr);
            std::tm tm{};
#ifdef _WIN32
            localtime_s(&tm, &t);
#else
            localtime_r(&t, &tm);
#endif
            std::ostringstream oss;
            oss << std::put_time(&tm, "%H:%M:%S");
            return oss.str();
        }
    }

    AppContext::AppContext()
        : m_settingsPath("../config/settings.json"),
          m_settings(m_settingsPath)
    {
    }

    AppContext::~AppContext() = default;

    void AppContext::initialize()
    {
        m_settings.load();
        m_settings.applyCalculationToAppState();
        m_animator.setSpeed(m_settings.menu().animationSpeed);
        emitLog("Pearl Calculator GUI 启动。", widgets::ToastKind::Info);
    }

    void AppContext::update(double deltaTime)
    {
        m_animator.update(deltaTime);
    }

    void AppContext::emitLog(const std::string& text, widgets::ToastKind kind)
    {
        m_console.push_back({nowStamp(), text, kind});
        if (m_console.size() > 500) m_console.pop_front();
        m_statusMessage = text;
    }

    void AppContext::applyMenuSettings()
    {
        const auto& m = m_settings.menu();
        if (m.theme == ThemeMode::Dark)
            applyDarkTheme(m.cornerRadius, m.windowOpacity);
        else
            applyLightTheme(m.cornerRadius, m.windowOpacity);
        setLanguage(m.language);
        m_animator.setSpeed(m.animationSpeed);
        ImGui::GetIO().FontGlobalScale = m.uiScale;
    }

    void AppContext::runSolve()
    {
        double dx, dy = 0.0, dz;
        if (!parseDouble(m_form.targetX, dx)) { emitLog("X 坐标无效。", widgets::ToastKind::Error); return; }
        if (m_form.useY && !parseDouble(m_form.targetY, dy)) { emitLog("Y 坐标无效。", widgets::ToastKind::Error); return; }
        if (!parseDouble(m_form.targetZ, dz)) { emitLog("Z 坐标无效。", widgets::ToastKind::Error); return; }

        g_appState.destination = {dx, dy, dz};
        g_appState.targetYPosition = dy;
        g_appState.calculatorSettings.specifiesYPosition = m_form.useY;
        g_appState.calculatorSettings.accuracyPriority   = m_form.useY || m_settings.calc().accuracyPriority;
        g_appState.calculatorSettings.below1_21_1        = m_settings.calc().below1_21_1;

        const Vector2 dest2D = {dx, dz};
        m_plans = solveLaunchPlans(dest2D);
        m_trajectory.clear();
        m_selectedPlanIndex.reset();
        m_rightPanel = RightPanelMode::Plans;
        m_plansGenStart = std::chrono::steady_clock::now();

        if (m_plans.empty())
        {
            emitLog("未找到可行方案。", widgets::ToastKind::Warning);
        }
        else
        {
            emitLog("已生成 " + std::to_string(m_plans.size()) + " 条候选方案。", widgets::ToastKind::Success);
        }
    }

    void AppContext::selectPlan(int index)
    {
        if (m_plans.empty()) return;
        index = std::clamp(index, 0, static_cast<int>(m_plans.size()) - 1);
        m_selectedPlanIndex = index;

        const LaunchPlan& plan = m_plans[index];
        m_form.redTnt    = std::to_string(plan.configuration.redTnt);
        m_form.blueTnt   = std::to_string(plan.configuration.blueTnt);
        m_form.direction = plan.configuration.direction;

        emitLog("已选中方案 #" + std::to_string(index + 1), widgets::ToastKind::Info);
    }

    void AppContext::runSimulationFromForm()
    {
        int red, blue;
        if (!parseInt(m_form.redTnt, red) || red < 0)   { emitLog("Red TNT 无效。", widgets::ToastKind::Error); return; }
        if (!parseInt(m_form.blueTnt, blue) || blue < 0){ emitLog("Blue TNT 无效。", widgets::ToastKind::Error); return; }
        if (!isValidDirection(m_form.direction))         { emitLog("方向无效。", widgets::ToastKind::Error); return; }

        int simulateUntilTick = 80;
        if (m_selectedPlanIndex.has_value() && !m_plans.empty())
        {
            simulateUntilTick = m_plans[*m_selectedPlanIndex].arrivalGameTick
                              + m_settings.calc().extraSimulationTicks;
        }

        m_trajectory = simulatePearlTrajectory({red, blue, m_form.direction}, simulateUntilTick);
        m_rightPanel = RightPanelMode::Trajectory;
        m_selectedTrajIndex.reset();
        m_trajGenStart = std::chrono::steady_clock::now();
        emitLog("已生成 " + std::to_string(m_trajectory.size()) + " 个 tick 的轨迹。", widgets::ToastKind::Success);
    }

    double AppContext::secondsSincePlansGen() const
    {
        if (m_plansGenStart.time_since_epoch().count() == 0) return 1e9;
        return std::chrono::duration<double>(std::chrono::steady_clock::now() - m_plansGenStart).count();
    }

    double AppContext::secondsSinceTrajGen() const
    {
        if (m_trajGenStart.time_since_epoch().count() == 0) return 1e9;
        return std::chrono::duration<double>(std::chrono::steady_clock::now() - m_trajGenStart).count();
    }

    void AppContext::resetPlansGenTimer()
    {
        m_plansGenStart = std::chrono::steady_clock::now();
    }

    void AppContext::resetTrajGenTimer()
    {
        m_trajGenStart = std::chrono::steady_clock::now();
    }

    void AppContext::resetForm()
    {
        m_form = CalculatorForm{};
        m_plans.clear();
        m_trajectory.clear();
        m_selectedPlanIndex.reset();
        m_selectedTrajIndex.reset();
        m_rightPanel = RightPanelMode::Plans;
        emitLog("已恢复默认输入。", widgets::ToastKind::Info);
    }
}
