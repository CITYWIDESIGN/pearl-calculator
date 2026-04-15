# PearlCalculator

[**中文**](./README_zh.md)

A high-performance Minecraft Ender Pearl trajectory simulator and TNT momentum calculator developed in C++.

## Core Features

* **TNT Quantity Calculation**: Automatically solves for the required amount of TNT in Red/Blue arrays based on target
  coordinates.
* **Pearl Trajectory Simulation**: Simulates the flight path of an Ender Pearl based on input configurations.
* **Configuration**: Fully compatible with configuration files generated
  by [PearlCalculatorCore](https://github.com/LegendsOfSky/PearlCalculatorCore). The program automatically loads
  parameters from `config.json` upon startup.

## Development Roadmap

* **Constraint-Based Calculation**: Support for specifying target Y-coordinates or flight time (Ticks) when calculating
  TNT quantities.
* **Robust Configuration**: Automatically fill missing fields with default values if the configuration file is
  incomplete, without overwriting the original file.

## Technical Specifications

* **Language**: C++ 17
* **Build System**: CMake 3.10+
* **Dependencies**: `nlohmann/json`

## License

This project is licensed under the [MIT License](LICENSE).