cmake_minimum_required(VERSION 3.20)
project(PearlCalculator)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(ftxui
        GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
        GIT_TAG v6.1.9
)
FetchContent_MakeAvailable(ftxui)

set(SOURCES
        main.cpp
        data.cpp
        data.h
        CLI/run_cli.cpp
        CLI/run_cli.h
        CLI/cli_common.cpp
        CLI/cli_common.h
        CLI/cli_simulation.cpp
        CLI/cli_simulation.h
        CLI/cli_calculation.cpp
        CLI/cli_calculation.h
        TUI/run_tui.cpp
        TUI/run_tui.h
        TUI/tui_app.cpp
        TUI/tui_app.h
        TUI/tui_state.cpp
        TUI/tui_state.h
        TUI/tui_view.cpp
        TUI/tui_view.h
        calcTNT/calculateTNT.cpp
        calcTNT/calculateTNT.h
        calcTNT/assignTNT.cpp
        calcTNT/assignTNT.h
        calcTNT/solve.cpp
        calcTNT/solve.h
        simulationPearl/calculatePearlInitialMotion.cpp
        simulationPearl/calculatePearlInitialMotion.h
        simulationPearl/simulatePearl.cpp
        simulationPearl/simulatePearl.h
        utils/createConfig.cpp
        utils/createConfig.h
        utils/loadConfig.cpp
        utils/loadConfig.h
        utils/getDistance3D.cpp
        utils/getDistance3D.h
        utils/calculateSingleTNTContribution.cpp
        utils/calculateSingleTNTContribution.h
        utils/initialize.cpp
        utils/initialize.h
        utils/maximumRange.cpp
        utils/maximumRange.h
        utils/clearScreen.cpp
        utils/clearScreen.h
        debug/displayData.cpp
        debug/displayData.h
        debug/displaySimulateResult.cpp
        debug/displaySimulateResult.h
        debug/displaySolve.cpp
        debug/displaySolve.h
)

add_executable(PearlCalculator ${SOURCES})

target_include_directories(PearlCalculator PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(PearlCalculator
        PRIVATE ftxui::screen
        PRIVATE ftxui::dom
        PRIVATE ftxui::component
)
