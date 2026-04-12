cmake_minimum_required(VERSION 4.0)
project(PearlCalculator)

set(CMAKE_CXX_STANDARD 20)

add_executable(PearlCalculator main.cpp
        utils/createConfig.cpp
        utils/createConfig.h
        utils/loadConfig.cpp
        utils/loadConfig.h
        data.h
        debug/displayData.cpp
        debug/displayData.h
        data.cpp
        utils/getDistance3D.cpp
        utils/getDistance3D.h
        utils/calculateSingleTNTContribution.cpp
        utils/calculateSingleTNTContribution.h
        utils/initialize.cpp
        utils/initialize.h
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
        debug/displaySimulateResult.cpp
        debug/displaySimulateResult.h
        utils/maximumRange.cpp
        utils/maximumRange.h
        debug/displaySolve.cpp
        debug/displaySolve.h
        CLI/run_cli.cpp
        CLI/run_cli.h)
