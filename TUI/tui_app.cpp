//
// Created by cT on 2026/4/15.
//

#include "tui_app.h"

#include <algorithm>
#include <exception>
#include <iostream>

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/mouse.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#include "tui_state.h"
#include "tui_view.h"

using namespace ftxui;

namespace
{
    constexpr int kContentHeight = 28;
    constexpr int kListVisibleRows = 13;

    bool isMouseInsideBox(const Box& box, const Mouse& mouse)
    {
        return mouse.x >= box.x_min && mouse.x <= box.x_max &&
            mouse.y >= box.y_min && mouse.y <= box.y_max;
    }

    bool isMouseInsideActiveList(const TuiState& state, const Mouse& mouse)
    {
        return state.rightPanelMode == TuiRightPanelMode::Plans
                   ? isMouseInsideBox(state.planListBox, mouse)
                   : isMouseInsideBox(state.trajectoryListBox, mouse);
    }

    void pageTuiPlanList(TuiState& state, const int delta)
    {
        const int maxOffset = std::max(0, static_cast<int>(state.planEntries.size()) - kListVisibleRows);
        state.planScrollOffset = std::clamp(state.planScrollOffset + delta, 0, maxOffset);
    }

    void pageTuiTrajectoryList(TuiState& state, const int delta)
    {
        const int maxOffset = std::max(0, static_cast<int>(state.trajectoryEntries.size()) - kListVisibleRows);
        state.trajectoryScrollOffset = std::clamp(state.trajectoryScrollOffset + delta, 0, maxOffset);
    }

    bool handleTuiListMouseSelection(TuiState& state, const Mouse& mouse)
    {
        if (state.rightPanelMode != TuiRightPanelMode::Plans || state.plans.empty())
        {
            return false;
        }

        if (!isMouseInsideBox(state.planListBox, mouse))
        {
            return false;
        }

        const int rowIndex = mouse.y - state.planListBox.y_min;
        const int absoluteIndex = state.planScrollOffset + rowIndex;
        if (absoluteIndex < 0 || absoluteIndex >= static_cast<int>(state.plans.size()))
        {
            return false;
        }

        selectTuiPlanByIndex(state, absoluteIndex);
        return true;
    }

    void ensureHighlightedPlanVisible(TuiState& state)
    {
        if (state.highlightedPlanIndex < state.planScrollOffset)
        {
            state.planScrollOffset = state.highlightedPlanIndex;
            return;
        }

        const int bottomVisibleIndex = state.planScrollOffset + kListVisibleRows - 1;
        if (state.highlightedPlanIndex > bottomVisibleIndex)
        {
            state.planScrollOffset = state.highlightedPlanIndex - kListVisibleRows + 1;
        }
    }

    Component buildTuiApplication(TuiState& state, ScreenInteractive& screen)
    {
        auto startButton = Button(
            "[ 开始计算 ]",
            [&]
            {
                state.page = TuiPage::Configuration;
                state.statusMessage = "请输入坐标，然后点击“计算方案”。";
            },
            makeTuiTextButtonStyle(false)
        );

        const InputOption coordinateInputStyle = makeTuiInputStyle(Color::CyanLight, Color::GreenLight);
        auto inputX = Input(&state.coordinates.x, "X", coordinateInputStyle);
        auto inputY = Input(&state.coordinates.y, "Y", coordinateInputStyle);
        auto inputZ = Input(&state.coordinates.z, "Z", coordinateInputStyle);
        auto inputRedTnt = Input(&state.coordinates.redTnt, "Red TNT", coordinateInputStyle);
        auto inputBlueTnt = Input(&state.coordinates.blueTnt, "Blue TNT", coordinateInputStyle);
        auto inputDirection = Input(&state.coordinates.direction, "Direction", coordinateInputStyle);

        auto toggleModeButton = Button(
            "[ 切换 2D / 3D ]",
            [&]
            {
                state.coordinates.showYField = !state.coordinates.showYField;
                state.statusMessage = state.coordinates.showYField
                                          ? "已切换到三维录入。"
                                          : "已切换到二维录入。";
            },
            makeTuiTextButtonStyle()
        );

        auto computeButton = Button(
            "[ 计算方案 ]",
            [&] { computeTuiPlans(state); },
            makeTuiTextButtonStyle()
        );

        auto startSimulationButton = Button(
            "[ S 开始模拟 ]",
            [&] { startTuiSimulation(state); },
            makeTuiTextButtonStyle()
        );

        auto resetButton = Button(
            "[ 重置 ]",
            [&] { resetTuiForm(state); },
            makeTuiTextButtonStyle()
        );

        auto leftPanel = Container::Vertical({
            inputX,
            inputY,
            inputZ,
            inputRedTnt,
            inputBlueTnt,
            inputDirection,
            toggleModeButton,
            computeButton,
            startSimulationButton,
            resetButton,
        });

        auto configurationContainer = Container::Vertical({
            leftPanel,
        });

        auto titlePage = Renderer(startButton, [startButton]
        {
            return vbox({
                    filler(),
                    renderTuiAsciiTitle(),
                    text(""),
                    text(""),
                    text("Powered By BotCity") | color(Color::GrayDark) | center,
                    filler(),
                    startButton->Render() | center,
                    filler(),
                })
                | size(HEIGHT, EQUAL, kContentHeight);
        });

        auto configurationPage = Renderer(
            configurationContainer,
            [
                &state,
                inputX,
                inputY,
                inputZ,
                inputRedTnt,
                inputBlueTnt,
                inputDirection,
                toggleModeButton,
                computeButton,
                resetButton,
                startSimulationButton
            ]
            {
                return renderTuiConfigurationLayout(
                    state,
                    inputX,
                    inputY,
                    inputZ,
                    inputRedTnt,
                    inputBlueTnt,
                    inputDirection,
                    toggleModeButton,
                    computeButton,
                    resetButton,
                    startSimulationButton,
                    state.planListBox,
                    state.trajectoryListBox
                );
            });

        auto root = Container::Tab({
                                       titlePage,
                                       configurationPage,
                                   }, &state.pageTabIndex);

        auto app = Renderer(root, [&state, titlePage, configurationPage]
        {
            state.pageTabIndex = state.page == TuiPage::Title ? 0 : 1;
            state.rightPanelTabIndex = state.rightPanelMode == TuiRightPanelMode::Plans ? 0 : 1;

            Element content = vbox({
                (state.pageTabIndex == 0 ? titlePage : configurationPage)->Render(),
                renderTuiStatusBar(state),
            });

            return vbox({
                    filler(),
                    hbox({
                        filler(),
                        wrapTuiShell(std::move(content)),
                        filler(),
                    }),
                    filler(),
                })
                | bgcolor(Color::Black);
        });

        return CatchEvent(app, [&state, &screen](Event event)
        {
            if (event.is_mouse() && event.mouse().motion == Mouse::Moved)
            {
                return true;
            }

            if (event == Event::Character("q") || event == Event::Character("Q"))
            {
                screen.Exit();
                return true;
            }

            if (state.page == TuiPage::Configuration)
            {
                if (event.is_mouse())
                {
                    const Mouse& mouse = event.mouse();
                    const bool isListMouseEvent = isMouseInsideActiveList(state, mouse);

                    if (mouse.button == Mouse::WheelDown)
                    {
                        if (!isListMouseEvent)
                        {
                            return false;
                        }

                        if (state.rightPanelMode == TuiRightPanelMode::Plans)
                        {
                            pageTuiPlanList(state, 5);
                        }
                        else
                        {
                            pageTuiTrajectoryList(state, 5);
                        }
                        return true;
                    }

                    if (mouse.button == Mouse::WheelUp)
                    {
                        if (!isListMouseEvent)
                        {
                            return false;
                        }

                        if (state.rightPanelMode == TuiRightPanelMode::Plans)
                        {
                            pageTuiPlanList(state, -5);
                        }
                        else
                        {
                            pageTuiTrajectoryList(state, -5);
                        }
                        return true;
                    }

                    if (mouse.button == Mouse::Left && mouse.motion == Mouse::Pressed)
                    {
                        if (handleTuiListMouseSelection(state, mouse))
                        {
                            return true;
                        }
                    }
                }

                if (event == Event::Character("s") || event == Event::Character("S"))
                {
                    startTuiSimulation(state);
                    return true;
                }

                if (event == Event::Character("e") || event == Event::Character("E"))
                {
                    state.rightPanelMode = TuiRightPanelMode::Plans;
                    state.statusMessage = "已返回方案列表。";
                    return true;
                }

                if (event == Event::ArrowDown && state.rightPanelMode == TuiRightPanelMode::Plans && !state.plans.
                    empty())
                {
                    state.highlightedPlanIndex = std::min(
                        state.highlightedPlanIndex + 1,
                        static_cast<int>(state.plans.size()) - 1
                    );
                    ensureHighlightedPlanVisible(state);
                    return true;
                }

                if (event == Event::ArrowUp && state.rightPanelMode == TuiRightPanelMode::Plans && !state.plans.empty())
                {
                    state.highlightedPlanIndex = std::max(state.highlightedPlanIndex - 1, 0);
                    ensureHighlightedPlanVisible(state);
                    return true;
                }

                if (event == Event::Return && state.rightPanelMode == TuiRightPanelMode::Plans)
                {
                    selectTuiHighlightedPlan(state);
                    return true;
                }
            }

            return false;
        });
    }
}

void runTuiApplication()
{
    try
    {
        TuiState state;
        initializeTuiResultEntries(state);

        ScreenInteractive screen = ScreenInteractive::Fullscreen();
        Component app = buildTuiApplication(state, screen);
        screen.Loop(app);
    }
    catch (const std::exception& exception)
    {
        std::cerr << "TUI 运行失败: " << exception.what() << std::endl;
        std::cerr << "按回车键结束..." << std::endl;
        std::cin.get();
    }
    catch (...)
    {
        std::cerr << "TUI 运行失败: 未知错误" << std::endl;
        std::cerr << "按回车键结束..." << std::endl;
        std::cin.get();
    }
}
