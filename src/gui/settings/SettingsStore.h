#ifndef PEARLCALCULATOR_GUI_SETTINGS_STORE_H
#define PEARLCALCULATOR_GUI_SETTINGS_STORE_H

#include <string>

#include "../i18n/I18n.h"

namespace pearl::gui
{
    enum class ThemeMode
    {
        Dark = 0,
        Light = 1
    };

    // 仅影响 GUI 外观 / 体验。
    struct MenuSettings
    {
        ThemeMode theme = ThemeMode::Dark;
        Language language = Language::ChineseSimp;

        std::string uiFont = "Microsoft YaHei UI";
        std::string monoFont = "JetBrains Mono";
        float fontSize = 16.0f;

        float cornerRadius = 8.0f;
        float animationSpeed = 1.0f;
        float uiScale = 1.0f;
        float windowOpacity = 0.96f;

        bool saveLayout = true;
    };

    // 仅影响算法。
    struct CalculationSettings
    {
        int  extraSimulationTicks = 10;     // 选中方案后，模拟到达后多模拟几 tick
        bool below1_21_1     = true;        // 1.21.1 以下规则
        bool accuracyPriority = true;       // 精度优先
    };

    class SettingsStore
    {
    public:
        explicit SettingsStore(std::string path);

        void load();
        void save() const;

        MenuSettings& menu() { return m_menu; }
        CalculationSettings& calc() { return m_calc; }

        [[nodiscard]] const MenuSettings& menu() const { return m_menu; }
        [[nodiscard]] const CalculationSettings& calc() const { return m_calc; }

        // 推送 calc 设置到 g_appState（包装 CalculatorSettings）。
        void applyCalculationToAppState() const;

    private:
        std::string m_path;
        MenuSettings m_menu;
        CalculationSettings m_calc;
    };
}

#endif
