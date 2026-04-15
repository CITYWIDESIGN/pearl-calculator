//
// Created by cT on 2026/4/15.
//

#include "tui_view.h"

#include <algorithm>
#include <utility>

using namespace ftxui;

namespace
{
    constexpr int kShellWidth = 120;
    constexpr int kShellHeight = 32;
    constexpr int kContentHeight = 28;
    constexpr int kLeftPanelWidth = 32;
    constexpr int kRightPanelHeight = 26;
    constexpr int kListVisibleRows = 13;
    constexpr int kRightPanelTextWidth = 82;
    constexpr int kListTextWidth = 76;
    constexpr int kInputRowWidth = 24;
    constexpr int kInputValueWidth = 14;

    std::string truncateText(const std::string& text, const std::size_t maxWidth)
    {
        if (text.size() <= maxWidth)
        {
            return text;
        }

        if (maxWidth <= 3)
        {
            return text.substr(0, maxWidth);
        }

        return text.substr(0, maxWidth - 3) + "...";
    }

    Element renderScrollableList(
        const std::vector<std::string>& entries,
        const int highlightedIndex,
        const std::optional<int> selectedIndex,
        const int scrollOffset,
        Box& listBox
    )
    {
        const int clampedOffset = std::clamp(
            scrollOffset,
            0,
            std::max(0, static_cast<int>(entries.size()) - kListVisibleRows)
        );
        const int endIndex = std::min(
            static_cast<int>(entries.size()),
            clampedOffset + kListVisibleRows
        );

        Elements rows;
        for (int index = clampedOffset; index < endIndex; ++index)
        {
            Element row = text(truncateText(entries[static_cast<std::size_t>(index)], kListTextWidth))
                | size(WIDTH, EQUAL, kListTextWidth);

            if (selectedIndex.has_value() && selectedIndex.value() == index)
            {
                row |= color(Color::GreenLight);
            }
            else
            {
                row |= color(Color::White);
            }

            if (highlightedIndex == index)
            {
                row |= bgcolor(Color::RGB(12, 28, 36));
                row |= color(Color::CyanLight);
                row |= bold;
            }

            rows.push_back(row);
        }

        while (static_cast<int>(rows.size()) < kListVisibleRows)
        {
            rows.push_back(text(" "));
        }

        return vbox(std::move(rows))
            | reflect(listBox)
            | size(HEIGHT, EQUAL, kListVisibleRows);
    }

    Element renderScrollBar(const int totalEntries, const int scrollOffset)
    {
        Elements rows;
        const int thumbRow = totalEntries <= kListVisibleRows
                                 ? 0
                                 : static_cast<int>(
                                     static_cast<float>(scrollOffset) /
                                     static_cast<float>(std::max(1, totalEntries - kListVisibleRows)) *
                                     static_cast<float>(kListVisibleRows - 1)
                                 );

        for (int row = 0; row < kListVisibleRows; ++row)
        {
            rows.push_back(
                row == thumbRow
                    ? text("█") | color(Color::GrayDark)
                    : text("│") | color(Color::GrayDark)
            );
        }

        return vbox(std::move(rows));
    }

    Element renderCenteredInputRow(
        const std::string& label,
        const Component& inputComponent
    )
    {
        return hbox({
                text(label) | color(Color::White) | size(WIDTH, EQUAL, 10),
                inputComponent->Render() | size(WIDTH, EQUAL, kInputValueWidth),
            })
            | size(WIDTH, EQUAL, kInputRowWidth)
            | center;
    }
}

InputOption makeTuiInputStyle(const Color idleColor, const Color activeColor)
{
    InputOption option = InputOption::Default();
    option.multiline = false;
    option.transform = [idleColor, activeColor](InputState state)
    {
        Element element = std::move(state.element);

        if (state.is_placeholder)
        {
            element |= dim;
        }

        element |= color(state.focused ? activeColor : idleColor);
        element |= bold;
        return element;
    };
    return option;
}

ButtonOption makeTuiTextButtonStyle(const bool highlightOnFocus)
{
    ButtonOption option;
    option.transform = [highlightOnFocus](const EntryState& state)
    {
        Element element = text(state.label);

        if (highlightOnFocus && (state.focused || state.active))
        {
            element |= color(Color::Black);
            element |= bgcolor(Color::CyanLight);
            element |= bold;
        }
        else
        {
            element |= color(Color::CyanLight);
        }

        return element;
    };
    return option;
}

Element renderTuiAsciiTitle()
{
    const std::vector<std::string> titleLines = {
        "██████  ███████  █████  ██████  ██      ",
        "██   ██ ██      ██   ██ ██   ██ ██      ",
        "██████  █████   ███████ ██████  ██      ",
        "██      ██      ██   ██ ██   ██ ██      ",
        "██      ███████ ██   ██ ██   ██ ███████ ",
        "",
        " ██████  █████  ██      ██████ ██    ██ ██       █████  ████████  ██████  ██████ ",
        "██      ██   ██ ██     ██      ██    ██ ██      ██   ██    ██    ██    ██ ██   ██",
        "██      ███████ ██     ██      ██    ██ ██      ███████    ██    ██    ██ ██████ ",
        "██      ██   ██ ██     ██      ██    ██ ██      ██   ██    ██    ██    ██ ██   ██",
        " ██████ ██   ██ ███████ ██████  ██████  ███████ ██   ██    ██     ██████  ██   ██",
    };

    Elements elements;
    for (const auto& line : titleLines)
    {
        elements.push_back(text(line) | color(Color::CyanLight) | bold | center);
    }

    return vbox(std::move(elements));
}

Element renderTuiStatusBar(const TuiState& state)
{
    std::string hint;

    if (state.page == TuiPage::Title)
    {
        hint = "[ Enter / Click ] Start  [ Q ] Quit";
    }
    else if (state.rightPanelMode == TuiRightPanelMode::Plans)
    {
        hint = "[ Click / Enter ] Select  [ Wheel ] Page  [ S ] Simulate";
    }
    else
    {
        hint = "[ Wheel ] Page  [ E ] Back To Plans";
    }

    return hbox({
        text(" MODE: KEYBOARD ") | color(Color::GreenLight) | bold,
        filler(),
        text(truncateText(hint, 58)) | color(Color::GrayDark),
    });
}

static Element renderTuiRightPanel(
    const TuiState& state,
    Box& planListBox,
    Box& trajectoryListBox
)
{
    const bool showPlans = state.rightPanelMode == TuiRightPanelMode::Plans;
    const std::string title = showPlans ? "[ 计算结果 ]" : "[ 珍珠模拟 ]";
    const std::string subtitle = showPlans
                                     ? "滚轮翻页，左键或回车选中方案。"
                                     : "展示完整飞行轨迹，滚轮翻页浏览。";

    const Element listElement = showPlans
                                    ? renderScrollableList(
                                        state.planEntries,
                                        state.highlightedPlanIndex,
                                        state.selectedPlanIndex,
                                        state.planScrollOffset,
                                        planListBox
                                    )
                                    : renderScrollableList(
                                        state.trajectoryEntries,
                                        state.highlightedTrajectoryIndex,
                                        std::nullopt,
                                        state.trajectoryScrollOffset,
                                        trajectoryListBox
                                    );

    const int totalEntries = showPlans
                                 ? static_cast<int>(state.planEntries.size())
                                 : static_cast<int>(state.trajectoryEntries.size());
    const int scrollOffset = showPlans ? state.planScrollOffset : state.trajectoryScrollOffset;

    return vbox({
            text(title) | color(showPlans ? Color::MagentaLight : Color::GreenLight) | bold,
            separatorEmpty(),
            text(truncateText(subtitle, kRightPanelTextWidth)) | color(Color::GrayDark),
            separatorEmpty(),
            hbox({
                listElement | flex,
                separatorEmpty(),
                renderScrollBar(totalEntries, scrollOffset),
            }) | flex,
            separatorEmpty(),
            text(truncateText(state.statusMessage, kRightPanelTextWidth)) | color(Color::White),
        })
        | size(HEIGHT, EQUAL, kRightPanelHeight)
        | flex
        | bgcolor(Color::Black);
}

Element renderTuiConfigurationLayout(
    const TuiState& state,
    const Component& inputX,
    const Component& inputY,
    const Component& inputZ,
    const Component& inputRedTnt,
    const Component& inputBlueTnt,
    const Component& inputDirection,
    const Component& toggleModeButton,
    const Component& computeButton,
    const Component& resetButton,
    const Component& startSimulationButton,
    Box& planListBox,
    Box& trajectoryListBox
)
{
    Elements leftElements;
    leftElements.push_back(text("[ 坐标录入 ]") | color(Color::YellowLight) | bold | center);
    leftElements.push_back(separatorEmpty());
    leftElements.push_back(toggleModeButton->Render() | center);
    leftElements.push_back(separatorEmpty());
    leftElements.push_back(separatorEmpty());
    leftElements.push_back(renderCenteredInputRow("X:", inputX));
    leftElements.push_back(separatorEmpty());

    if (state.coordinates.showYField)
    {
        leftElements.push_back(renderCenteredInputRow("Y:", inputY));
        leftElements.push_back(separatorEmpty());
    }

    leftElements.push_back(renderCenteredInputRow("Z:", inputZ));
    leftElements.push_back(separatorEmpty());
    leftElements.push_back(renderCenteredInputRow("Red TNT:", inputRedTnt));
    leftElements.push_back(separatorEmpty());
    leftElements.push_back(renderCenteredInputRow("Blue TNT:", inputBlueTnt));
    leftElements.push_back(separatorEmpty());
    leftElements.push_back(renderCenteredInputRow("Direction:", inputDirection));
    leftElements.push_back(separatorEmpty());
    leftElements.push_back(separatorEmpty());
    leftElements.push_back(separatorEmpty());
    leftElements.push_back(separatorEmpty());
    leftElements.push_back(computeButton->Render() | center);
    leftElements.push_back(separatorEmpty());
    leftElements.push_back(startSimulationButton->Render() | center);
    leftElements.push_back(separatorEmpty());
    leftElements.push_back(resetButton->Render() | center);
    leftElements.push_back(separatorEmpty());
    leftElements.push_back(
        text(state.coordinates.showYField ? "当前模式: 3D" : "当前模式: 2D")
        | color(Color::GrayDark)
        | center
    );

    Element leftPanel = vbox({
            vbox(std::move(leftElements)) | center,
            filler(),
        })
        | size(WIDTH, EQUAL, kLeftPanelWidth);

    return hbox({
            leftPanel,
            separatorDouble(),
            renderTuiRightPanel(state, planListBox, trajectoryListBox),
        })
        | size(HEIGHT, EQUAL, kContentHeight);
}

Element wrapTuiShell(Element child)
{
    return borderDouble(std::move(child))
        | size(WIDTH, EQUAL, kShellWidth)
        | size(HEIGHT, EQUAL, kShellHeight)
        | color(Color::White)
        | bgcolor(Color::Black);
}