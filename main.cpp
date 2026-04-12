#include <iostream>
#include <vector>

#include "utils/initialize.h"
#include "simulationPearl/simulatePearl.h"
#include "calcTNT/solve.h"
#include "data.h"

#include "debug/displaySimulateResult.h"
#include "debug/displaySolve.h"

int main()
{
    // 初始化
    initialize();

    vector2 destination = {
        1145,
        114
    };

    const std::vector<Plan> result_solve = solve(destination);
    displaySolve(result_solve,destination);

    const std::vector<Simulate> result_simulatePearl = simulatePearl(result_solve[0].Result,10);
    displaySimulateResult(result_simulatePearl);

    return 0;
}
