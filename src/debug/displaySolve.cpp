//
// Created by cT on 2026/4/11.
//

#include "displaySolve.h"

#include <iomanip>
#include <iostream>
#include <vector>

void displaySolutions(const std::vector<LaunchPlan>& candidates, const Vector2 destination)
{
    if (candidates.empty())
    {
        std::cout << "\n[!] No candidates found for (" << destination.x << ", " << destination.z << ")\n" << std::endl;
        return;
    }

    std::cout << "\nTarget: " << destination.x << ", " << destination.z << "\n";
    std::cout << "Rank\tOffset\t\tTick\t\tRed\t\tBlue\t\tDir\n" << std::endl;
    std::cout << std::fixed << std::setprecision(8);

    int displayCount = 0;

    for (const auto& candidate : candidates)
    {
        std::cout << displayCount + 1 << "\t"
            << candidate.landingOffset << "\t"
            << candidate.arrivalGameTick << "\t\t"
            << candidate.configuration.redTnt << "\t\t"
            << candidate.configuration.blueTnt << "\t\t"
            << candidate.configuration.direction << "\n";

        if (++displayCount >= 15)
        {
            break;
        }
    }

    // 保留原有的结果截断展示逻辑。
    if (candidates.size() > 15)
    {
        std::cout << "\n... [Omitted " << candidates.size() - 15 << " results]\n";
    }

    std::cout << std::endl;
}