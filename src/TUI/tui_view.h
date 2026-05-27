//
// Created by cT on 2026/4/15.
//

#ifndef PEARLCALCULATOR_TUI_VIEW_H
#define PEARLCALCULATOR_TUI_VIEW_H

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>

#include "tui_state.h"

ftxui::InputOption makeTuiInputStyle(ftxui::Color idleColor, ftxui::Color activeColor);
ftxui::ButtonOption makeTuiTextButtonStyle(bool highlightOnFocus = true);
ftxui::Element renderTuiAsciiTitle();
ftxui::Element renderTuiStatusBar(const TuiState& state);
ftxui::Element renderTuiConfigurationLayout(
    const TuiState& state,
    const ftxui::Component& inputX,
    const ftxui::Component& inputY,
    const ftxui::Component& inputZ,
    const ftxui::Component& inputRedTnt,
    const ftxui::Component& inputBlueTnt,
    const ftxui::Component& inputDirection,
    const ftxui::Component& toggleModeButton,
    const ftxui::Component& computeButton,
    const ftxui::Component& resetButton,
    const ftxui::Component& startSimulationButton,
    ftxui::Box& planListBox,
    ftxui::Box& trajectoryListBox
);
ftxui::Element wrapTuiShell(ftxui::Element child);

#endif // PEARLCALCULATOR_TUI_VIEW_H