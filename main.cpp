#include <iostream>
#include <vector>

#include "utils/initialize.h"
#include "CLI/run_cli.h"
#include "simulationPearl/simulatePearl.h"
#include "calcTNT/solve.h"
#include "data.h"

#include "debug/displaySimulateResult.h"
#include "debug/displaySolve.h"

int main()
{
    // 初始化
    initialize();

    // 以 cli 命令行模式运行
    run_cli();

    return 0;
}