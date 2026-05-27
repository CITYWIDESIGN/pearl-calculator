#include <cstring>
#include <exception>
#include <iostream>

#include "CLI/run_cli.h"
#include "TUI/run_tui.h"
#include "gui/run_gui.h"
#include "utils/initialize.h"

namespace
{
    enum class Mode { Gui, Tui, Cli };

    Mode pickMode(int argc, char** argv)
    {
        for (int i = 1; i < argc; ++i)
        {
            if (std::strcmp(argv[i], "--cli") == 0) return Mode::Cli;
            if (std::strcmp(argv[i], "--tui") == 0) return Mode::Tui;
            if (std::strcmp(argv[i], "--gui") == 0) return Mode::Gui;
        }
        return Mode::Gui;
    }
}

int main(int argc, char** argv)
{
    try
    {
        initializeApplication();

        switch (pickMode(argc, argv))
        {
            case Mode::Cli: runCli(); break;
            case Mode::Tui: runTui(); break;
            case Mode::Gui: return runGui();
        }
    }
    catch (const std::exception& exception)
    {
        std::cerr << "程序异常退出: " << exception.what() << std::endl;
        std::cerr << "按回车键结束..." << std::endl;
        std::cin.get();
        return 1;
    }
    catch (...)
    {
        std::cerr << "程序异常退出: 未知错误" << std::endl;
        std::cerr << "按回车键结束..." << std::endl;
        std::cin.get();
        return 1;
    }

    return 0;
}
