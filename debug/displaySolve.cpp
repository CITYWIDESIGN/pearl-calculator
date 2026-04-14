//
// Created by cT on 2026/4/11.
//
#include <iostream>
#include <vector>
#include <iomanip>

#include "displaySolve.h"
#include "../data.h"

void displaySolve(const std::vector<Plan>& candidates, vector2 destination) {
    if (candidates.empty()) {
        std::cout << "\n[!] No candidates found for (" << destination.X << ", " << destination.Z << ")\n" << std::endl;
        return;
    }

    std::cout << "\nTarget: " << destination.X << ", " << destination.Z << "\n";
    std::cout << "Rank\tOffset\t\tTick\t\tRed\t\tBlue\t\tDir\n" << std::endl;

    int count = 0;
    std::cout << std::fixed << std::setprecision(8);
    for (const auto& p : candidates) {
        std::cout << count + 1 << "\t"
                  << p.landingOffset << "\t"
                  << p.arriveGameTick << "\t\t"
                  << p.Result.redTNT << "\t\t"
                  << p.Result.blueTNT << "\t\t"
                  << p.Result.direction << "\n";

        if (++count >= 15) break;
    }

    // 打印省略提示
    if (candidates.size() > 15) {
        std::cout << "\n... [Omitted " << candidates.size() - 15 << " results]\n";
    }

    std::cout << std::endl;
}