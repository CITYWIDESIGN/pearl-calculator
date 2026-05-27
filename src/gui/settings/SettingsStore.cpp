#include "SettingsStore.h"

#include <fstream>
#include <utility>

#include <nlohmann/json.hpp>

#include "../../data.h"

namespace pearl::gui
{
    using json = nlohmann::json;

    SettingsStore::SettingsStore(std::string path)
        : m_path(std::move(path))
    {
    }

    void SettingsStore::load()
    {
        std::ifstream file(m_path);
        if (!file.is_open())
        {
            return;
        }

        json document;
        try
        {
            file >> document;
        }
        catch (...)
        {
            return;
        }

        if (document.contains("menu"))
        {
            const auto& m = document["menu"];
            if (m.contains("theme"))           m_menu.theme         = static_cast<ThemeMode>(m["theme"].get<int>());
            if (m.contains("language"))        m_menu.language      = static_cast<Language>(m["language"].get<int>());
            if (m.contains("uiFont"))          m_menu.uiFont        = m["uiFont"].get<std::string>();
            if (m.contains("monoFont"))        m_menu.monoFont      = m["monoFont"].get<std::string>();
            if (m.contains("fontSize"))        m_menu.fontSize      = m["fontSize"].get<float>();
            if (m.contains("cornerRadius"))    m_menu.cornerRadius  = m["cornerRadius"].get<float>();
            if (m.contains("animationSpeed")) m_menu.animationSpeed = m["animationSpeed"].get<float>();
            if (m.contains("uiScale"))         m_menu.uiScale       = m["uiScale"].get<float>();
            if (m.contains("windowOpacity"))   m_menu.windowOpacity = m["windowOpacity"].get<float>();
            if (m.contains("saveLayout"))      m_menu.saveLayout    = m["saveLayout"].get<bool>();
        }

        if (document.contains("calc"))
        {
            const auto& c = document["calc"];
            if (c.contains("extraSimulationTicks"))   m_calc.extraSimulationTicks = c["extraSimulationTicks"].get<int>();
            if (c.contains("below1_21_1"))            m_calc.below1_21_1          = c["below1_21_1"].get<bool>();
            if (c.contains("accuracyPriority"))       m_calc.accuracyPriority     = c["accuracyPriority"].get<bool>();
        }
    }

    void SettingsStore::save() const
    {
        json document;
        document["menu"] = {
            {"theme",          static_cast<int>(m_menu.theme)},
            {"language",       static_cast<int>(m_menu.language)},
            {"uiFont",         m_menu.uiFont},
            {"monoFont",       m_menu.monoFont},
            {"fontSize",       m_menu.fontSize},
            {"cornerRadius",   m_menu.cornerRadius},
            {"animationSpeed", m_menu.animationSpeed},
            {"uiScale",        m_menu.uiScale},
            {"windowOpacity",  m_menu.windowOpacity},
            {"saveLayout",     m_menu.saveLayout},
        };
        document["calc"] = {
            {"extraSimulationTicks", m_calc.extraSimulationTicks},
            {"below1_21_1",          m_calc.below1_21_1},
            {"accuracyPriority",     m_calc.accuracyPriority},
        };

        std::ofstream out(m_path);
        if (!out.is_open()) return;
        out << document.dump(2);
    }

    void SettingsStore::applyCalculationToAppState() const
    {
        g_appState.calculatorSettings.below1_21_1      = m_calc.below1_21_1;
        g_appState.calculatorSettings.accuracyPriority = m_calc.accuracyPriority;
    }
}
