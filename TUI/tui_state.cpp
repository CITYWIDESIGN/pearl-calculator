//
// Created by cT on 2026/4/15.
//

#include "tui_state.h"

#include <algorithm>
#include <iomanip>
#include <sstream>

#include "../calcTNT/solve.h"
#include "../data.h"
#include "../simulationPearl/simulatePearl.h"

constexpr int COL_WIDTHS[] = {6, 18, 11, 8, 8, 10};
constexpr int POI_WIDTHS[] = {11, 12, 12, 12, 12, 12, 12};

namespace {
    constexpr int kListVisibleRows = 22;

    std::string formatDouble(const double value, const int precision) {
        std::ostringstream stream;
        stream << std::fixed << std::setprecision(precision) << value;
        return stream.str();
    }

    bool parseCoordinate(const std::string &text, double &value) {
        try {
            std::size_t parsedLength = 0;
            value = std::stod(text, &parsedLength);
            return parsedLength == text.size();
        } catch (...) {
            return false;
        }
    }

    bool parseIntegerValue(const std::string &text, int &value) {
        try {
            std::size_t parsedLength = 0;
            value = std::stoi(text, &parsedLength);
            return parsedLength == text.size();
        } catch (...) {
            return false;
        }
    }

    bool isValidDirectionText(const std::string &direction) {
        return direction == "North" ||
               direction == "South" ||
               direction == "East" ||
               direction == "West";
    }
}

void initializeTuiResultEntries(TuiState &state) {
    state.planEntries = {"暂无计算结果。请先输入坐标并执行计算。"};
    state.trajectoryEntries = {"暂无模拟轨迹。请先选择方案并开始模拟。"};
    state.highlightedPlanIndex = 0;
    state.highlightedTrajectoryIndex = 0;
    state.planScrollOffset = 0;
    state.trajectoryScrollOffset = 0;
    state.selectedPlanIndex.reset();
}

void rebuildTuiPlanEntries(TuiState &state) {
    state.planEntries.clear();

    for (std::size_t i = 0; i < state.plans.size(); ++i) {
        const auto &p = state.plans[i];
        std::ostringstream oss;

        oss << std::left << std::setw(COL_WIDTHS[0]) << (i + 1)
            << std::left << std::setw(COL_WIDTHS[1]) << std::fixed << std::setprecision(5) << p.landingOffset
            << std::left << std::setw(COL_WIDTHS[2]) << p.arrivalGameTick
            << std::left << std::setw(COL_WIDTHS[3]) << p.configuration.redTnt
            << std::left << std::setw(COL_WIDTHS[4]) << p.configuration.blueTnt
            << " " << p.configuration.direction;
        state.planEntries.push_back(oss.str());
    }

    if (state.planEntries.empty()) {
        state.planEntries.push_back("暂无计算结果。请先输入坐标并执行计算。");
        state.highlightedPlanIndex = 0;
        state.planScrollOffset = 0;
        state.selectedPlanIndex.reset();
        return;
    }

    state.highlightedPlanIndex = std::clamp(
        state.highlightedPlanIndex,
        0,
        static_cast<int>(state.planEntries.size()) - 1
    );

    if (!state.selectedPlanIndex.has_value()) {
        state.selectedPlanIndex = 0;
    }

    state.planScrollOffset = std::clamp(
        state.planScrollOffset,
        0,
        std::max(0, static_cast<int>(state.planEntries.size()) - kListVisibleRows)
    );
}

void rebuildTuiTrajectoryEntries(TuiState &state) {
    state.trajectoryEntries.clear();

    for (const auto& pt : state.trajectory) {
        std::ostringstream oss;

        oss << std::left << std::setw(POI_WIDTHS[0]) << pt.gameTick
            << std::left << std::setw(POI_WIDTHS[1]) << std::fixed << std::setprecision(5) << pt.position.x
            << std::left << std::setw(POI_WIDTHS[2]) << pt.position.y
            << std::left << std::setw(POI_WIDTHS[3]) << pt.position.z
            << std::left << std::setw(POI_WIDTHS[4]) << pt.motion.x
            << std::left << std::setw(POI_WIDTHS[5]) << pt.motion.y
            << " " << pt.motion.z;
        state.trajectoryEntries.push_back(oss.str());
    }

    if (state.trajectoryEntries.empty()) {
        state.trajectoryEntries.push_back("暂无模拟轨迹。请先选择方案并开始模拟。");
        state.highlightedTrajectoryIndex = 0;
        state.trajectoryScrollOffset = 0;
        return;
    }

    state.highlightedTrajectoryIndex = std::clamp(
        state.highlightedTrajectoryIndex,
        0,
        static_cast<int>(state.trajectoryEntries.size()) - 1
    );
    state.trajectoryScrollOffset = std::clamp(
        state.trajectoryScrollOffset,
        0,
        std::max(0, static_cast<int>(state.trajectoryEntries.size()) - kListVisibleRows)
    );
}

bool applyTuiCoordinateConfiguration(TuiState &state) {
    double destinationX = 0.0;
    double destinationY = 0.0;
    double destinationZ = 0.0;

    if (!parseCoordinate(state.coordinates.x, destinationX)) {
        state.statusMessage = "X 坐标格式无效。";
        return false;
    }

    if (state.coordinates.showYField && !parseCoordinate(state.coordinates.y, destinationY)) {
        state.statusMessage = "Y 坐标格式无效。";
        return false;
    }

    if (!parseCoordinate(state.coordinates.z, destinationZ)) {
        state.statusMessage = "Z 坐标格式无效。";
        return false;
    }

    g_appState.destination = {destinationX, destinationY, destinationZ};
    g_appState.targetYPosition = destinationY;
    g_appState.calculatorSettings.specifiesYPosition = state.coordinates.showYField;
    g_appState.calculatorSettings.accuracyPriority = state.coordinates.showYField;

    return true;
}

void computeTuiPlans(TuiState &state) {
    if (!applyTuiCoordinateConfiguration(state)) {
        return;
    }

    state.rightPanelMode = TuiRightPanelMode::Plans;
    state.trajectory.clear();
    rebuildTuiTrajectoryEntries(state);

    const Vector2 destination2D = {
        g_appState.destination.x,
        g_appState.destination.z
    };

    state.plans = solveLaunchPlans(destination2D);
    rebuildTuiPlanEntries(state);

    if (state.plans.empty()) {
        state.statusMessage = "没有找到可用方案，请检查坐标或模式。";
        return;
    }

    state.highlightedPlanIndex = 0;
    state.planScrollOffset = 0;
    state.selectedPlanIndex.reset();
    state.statusMessage = "已生成方案列表。左键或回车选择，按 S 开始模拟。";
}

void selectTuiHighlightedPlan(TuiState &state) {
    if (state.plans.empty()) {
        state.statusMessage = "当前没有可选方案。";
        return;
    }

    state.selectedPlanIndex = state.highlightedPlanIndex;

    const LaunchPlan &plan = state.plans[*state.selectedPlanIndex];
    state.coordinates.redTnt = std::to_string(plan.configuration.redTnt);
    state.coordinates.blueTnt = std::to_string(plan.configuration.blueTnt);
    state.coordinates.direction = plan.configuration.direction;
    std::ostringstream stream;
    stream << "已选中方案 #" << *state.selectedPlanIndex + 1
            << "  R " << plan.configuration.redTnt
            << "  B " << plan.configuration.blueTnt
            << "  " << plan.configuration.direction
            << "  GT " << plan.arrivalGameTick
            << "。按 S 开始模拟。";
    state.statusMessage = stream.str();
}

void selectTuiPlanByIndex(TuiState &state, const int index) {
    if (state.plans.empty()) {
        state.statusMessage = "当前没有可选方案。";
        return;
    }

    state.highlightedPlanIndex = std::clamp(index, 0, static_cast<int>(state.plans.size()) - 1);
    selectTuiHighlightedPlan(state);
}

void startTuiSimulation(TuiState &state) {
    int redTntCount = 0;
    int blueTntCount = 0;

    if (!parseIntegerValue(state.coordinates.redTnt, redTntCount) || redTntCount < 0) {
        state.statusMessage = "Red TNT 输入无效。";
        return;
    }

    if (!parseIntegerValue(state.coordinates.blueTnt, blueTntCount) || blueTntCount < 0) {
        state.statusMessage = "Blue TNT 输入无效。";
        return;
    }

    if (!isValidDirectionText(state.coordinates.direction)) {
        state.statusMessage = "方向输入无效，请使用 North/South/East/West。";
        return;
    }

    int simulateUntilTick = 80;
    if (state.selectedPlanIndex.has_value() && !state.plans.empty()) {
        simulateUntilTick = state.plans[*state.selectedPlanIndex].arrivalGameTick + 10;
    }

    // 这里优先使用当前输入框中的参数，允许用户在选中方案后继续手动微调并自定义模拟。
    state.trajectory = simulatePearlTrajectory(
        {redTntCount, blueTntCount, state.coordinates.direction},
        simulateUntilTick
    );
    rebuildTuiTrajectoryEntries(state);
    state.rightPanelMode = TuiRightPanelMode::Simulation;
    state.statusMessage = "已切换到珍珠模拟列表。按 E 返回方案列表。";
}

void resetTuiForm(TuiState &state) {
    state.coordinates = TuiCoordinateForm{};
    state.plans.clear();
    state.planEntries.clear();
    state.highlightedPlanIndex = 0;
    state.planScrollOffset = 0;
    state.selectedPlanIndex.reset();
    state.trajectory.clear();
    state.highlightedTrajectoryIndex = 0;
    state.trajectoryScrollOffset = 0;
    rebuildTuiPlanEntries(state);
    rebuildTuiTrajectoryEntries(state);
    state.rightPanelMode = TuiRightPanelMode::Plans;
    state.statusMessage = "已恢复默认输入。";
}
