#include <exception>
#include <iostream>

#include "CLI/run_cli.h"
#include "TUI/run_tui.h"
#include "utils/initialize.h"

int main()
{
    try
    {
        // 初始化运行时状态。
        initializeApplication();

        // 默认启动稳定的 TUI 入口。
        runTui();
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
