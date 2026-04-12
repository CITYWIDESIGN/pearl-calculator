# PearlCalculator

[**English**](./README.md)

基于 C++ 开发的高性能 Minecraft 末影珍珠轨迹模拟与 TNT 动力计算器。

## 核心功能
- **TNT 当量计算**：根据目标坐标自动求解红/蓝阵列所需的 TNT 数量。
- **珍珠轨迹模拟**：根据输入的配置模拟珍珠的飞行轨迹。
- **配置**：完全兼容 [PearlCalculatorCore](https://github.com/LegendsOfSky/PearlCalculatorCore) 生成的配置文件，程序启动后自动从 `config.json` 加载参数。

## 开发规划
- **约束计算**：支持在计算 TNT 数量时指定目标 Y 坐标或飞行时间 (Ticks)。
- **健壮性配置**：当配置文件缺失字段时，自动使用默认值补全而不覆盖原文件。

## 技术规格
- **开发语言**：C++ 17
- **构建系统**：CMake 3.10+
- **依赖库**：`nlohmann/json`

## 开源协议
本项目采用 [MIT License](LICENSE) 协议。
