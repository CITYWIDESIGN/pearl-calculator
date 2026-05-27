//
// Created by cT on 2026/4/15.
//

#ifndef PEARLCALCULATOR_TUI_STATE_H
#define PEARLCALCULATOR_TUI_STATE_H

#include <optional>
#include <string>
#include <vector>

#include <ftxui/screen/box.hpp>

#include "../data.h"

enum class TuiPage
{
    Title = 0,
    Configuration = 1
};

enum class TuiRightPanelMode
{
    Plans = 0,
    Simulation = 1
};

struct TuiCoordinateForm
{
    std::string x = "0";
    std::string y = "128";
    std::string z = "0";
    std::string redTnt = "0";
    std::string blueTnt = "0";
    std::string direction = "North";
    bool showYField = false;

    bool xNeedsSelect = false;
    bool yNeedsSelect = false;
    bool zNeedsSelect = false;
    bool redNeedsSelect = false;
    bool blueNeedsSelect = false;
    bool dirNeedsSelect = false;
};
struct TuiState
{
    TuiPage page = TuiPage::Title;
    TuiRightPanelMode rightPanelMode = TuiRightPanelMode::Plans;
    int pageTabIndex = 0;
    int rightPanelTabIndex = 0;
    TuiCoordinateForm coordinates;

    std::vector<LaunchPlan> plans;
    std::vector<std::string> planEntries;
    int highlightedPlanIndex = 0;
    std::optional<int> selectedPlanIndex;
    int planScrollOffset = 0;

    std::vector<SimulationPoint> trajectory;
    std::vector<std::string> trajectoryEntries;
    int highlightedTrajectoryIndex = 0;
    int trajectoryScrollOffset = 0;

    ftxui::Box planListBox;
    ftxui::Box trajectoryListBox;

    std::string statusMessage = "准备就绪。";
};

void initializeTuiResultEntries(TuiState& state);
void rebuildTuiPlanEntries(TuiState& state);
void rebuildTuiTrajectoryEntries(TuiState& state);
bool applyTuiCoordinateConfiguration(TuiState& state);
void computeTuiPlans(TuiState& state);
void selectTuiHighlightedPlan(TuiState& state);
void selectTuiPlanByIndex(TuiState& state, int index);
void startTuiSimulation(TuiState& state);
void resetTuiForm(TuiState& state);

#endif // PEARLCALCULATOR_TUI_STATE_H