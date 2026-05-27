cmake_minimum_required(VERSION 3.20)
project(PearlCalculator)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 💡 优化点 1：针对 MSVC 编译器，现代 CMake 官方推荐的强制静态链接 C/C++ 运行库(MT)的写法
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

include(FetchContent)

# ---------------------------------------------------------------------------
# FTXUI (TUI 模式仍保留)
# ---------------------------------------------------------------------------
FetchContent_Declare(ftxui
        GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
        GIT_TAG v6.1.9
)
FetchContent_MakeAvailable(ftxui)

# ---------------------------------------------------------------------------
# GLFW (GUI 窗口)
# ---------------------------------------------------------------------------
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
# 💡 优化点 2：强制 GLFW 编译为静态库，不允许其生成动态 dll
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(glfw
        GIT_REPOSITORY https://github.com/glfw/glfw
        GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw)

# ---------------------------------------------------------------------------
# Dear ImGui (docking 分支)
# ---------------------------------------------------------------------------
FetchContent_Declare(imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui
        GIT_TAG docking
)
FetchContent_MakeAvailable(imgui)

set(IMGUI_SOURCES
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
)
target_link_libraries(imgui PUBLIC glfw)

# ---------------------------------------------------------------------------
# OpenGL
# ---------------------------------------------------------------------------
find_package(OpenGL REQUIRED)

# ---------------------------------------------------------------------------
# 项目源文件
# ---------------------------------------------------------------------------
set(SOURCES
        src/main.cpp
        src/data.cpp
        src/data.h

        src/CLI/run_cli.cpp
        src/CLI/run_cli.h
        src/CLI/cli_common.cpp
        src/CLI/cli_common.h
        src/CLI/cli_simulation.cpp
        src/CLI/cli_simulation.h
        src/CLI/cli_calculation.cpp
        src/CLI/cli_calculation.h

        src/TUI/run_tui.cpp
        src/TUI/run_tui.h
        src/TUI/tui_app.cpp
        src/TUI/tui_app.h
        src/TUI/tui_state.cpp
        src/TUI/tui_state.h
        src/TUI/tui_view.cpp
        src/TUI/tui_view.h

        src/calcTNT/calculateTNT.cpp
        src/calcTNT/calculateTNT.h
        src/calcTNT/assignTNT.cpp
        src/calcTNT/assignTNT.h
        src/calcTNT/solve.cpp
        src/calcTNT/solve.h

        src/simulationPearl/calculatePearlInitialMotion.cpp
        src/simulationPearl/calculatePearlInitialMotion.h
        src/simulationPearl/simulatePearl.cpp
        src/simulationPearl/simulatePearl.h

        src/utils/createConfig.cpp
        src/utils/createConfig.h
        src/utils/loadConfig.cpp
        src/utils/loadConfig.h
        src/utils/getDistance3D.cpp
        src/utils/getDistance3D.h
        src/utils/calculateSingleTNTContribution.cpp
        src/utils/calculateSingleTNTContribution.h
        src/utils/initialize.cpp
        src/utils/initialize.h
        src/utils/maximumRange.cpp
        src/utils/maximumRange.h
        src/utils/clearScreen.cpp
        src/utils/clearScreen.h

        src/debug/displayData.cpp
        src/debug/displayData.h
        src/debug/displaySimulateResult.cpp
        src/debug/displaySimulateResult.h
        src/debug/displaySolve.cpp
        src/debug/displaySolve.h

        # ---- GUI ----
        src/gui/run_gui.cpp
        src/gui/run_gui.h
        src/gui/app/Application.cpp
        src/gui/app/Application.h
        src/gui/app/AppContext.cpp
        src/gui/app/AppContext.h
        src/gui/animation/Easing.cpp
        src/gui/animation/Easing.h
        src/gui/animation/Animator.cpp
        src/gui/animation/Animator.h
        src/gui/theme/Theme.cpp
        src/gui/theme/Theme.h
        src/gui/i18n/I18n.cpp
        src/gui/i18n/I18n.h
        src/gui/fonts/FontManager.cpp
        src/gui/fonts/FontManager.h
        src/gui/settings/SettingsStore.cpp
        src/gui/settings/SettingsStore.h
        src/gui/widgets/Card.cpp
        src/gui/widgets/Card.h
        src/gui/widgets/AnimatedTabBar.cpp
        src/gui/widgets/AnimatedTabBar.h
        src/gui/widgets/Toast.cpp
        src/gui/widgets/Toast.h
        src/gui/windows/CalculatorWindow.cpp
        src/gui/windows/CalculatorWindow.h
        src/gui/windows/SettingsWindow.cpp
        src/gui/windows/SettingsWindow.h
        src/gui/windows/ConsoleWindow.cpp
        src/gui/windows/ConsoleWindow.h
        src/gui/windows/AboutWindow.cpp
        src/gui/windows/AboutWindow.h
)

# 仅在 Release 编译时，启动 WIN32 窗口模式隐藏控制台
if(WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Release")
    add_executable(PearlCalculator WIN32 ${SOURCES})
else()
    add_executable(PearlCalculator ${SOURCES})
endif()

target_include_directories(PearlCalculator PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party
)

target_link_libraries(PearlCalculator
        PRIVATE ftxui::screen
        PRIVATE ftxui::dom
        PRIVATE ftxui::component
        PRIVATE imgui
        PRIVATE glfw
        PRIVATE OpenGL::GL
)

# ---------------------------------------------------------------------------
# 💡 优化点 3：现代且严格的平台及编译器链接选项定义
# ---------------------------------------------------------------------------
if(WIN32)
    if(MSVC)
        # MSVC Release 模式下消除控制台，并确保 main 入口正确挂载
        if(CMAKE_BUILD_TYPE STREQUAL "Release")
            target_link_options(PearlCalculator PRIVATE "/SUBSYSTEM:WINDOWS" "/ENTRY:mainCRTStartup")
        endif()
    else()
        # MinGW 环境：强制将 gcc/stdc++ 运行时全部静态打包，并针对 Release 消除控制台
        target_link_options(PearlCalculator PRIVATE "-static" "-static-libgcc" "-static-libstdc++")
        if(CMAKE_BUILD_TYPE STREQUAL "Release")
            target_link_options(PearlCalculator PRIVATE "-mwindows")
        endif()
    endif()
endif()