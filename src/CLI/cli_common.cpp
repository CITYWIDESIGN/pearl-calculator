//
// Created by cT on 2026/4/15.
//

#include "cli_common.h"

#include <iostream>
#include <limits>

void clearCliInputBuffer()
{
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void waitForCliEnter()
{
    std::cout << "\n按回车键继续..." << std::endl;

    if (std::cin.peek() == '\n')
    {
        std::cin.ignore();
    }

    std::cin.get();
}