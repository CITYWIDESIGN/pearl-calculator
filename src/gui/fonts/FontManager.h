#ifndef PEARLCALCULATOR_GUI_FONT_MANAGER_H
#define PEARLCALCULATOR_GUI_FONT_MANAGER_H

#include <string>
#include <vector>

#include <imgui.h>

namespace pearl::gui
{
    class FontManager
    {
    public:
        // 在 ImGui IO 字体表已就绪后调用；按 sizes 列表分别加载 UI / Mono 字体。
        void load(const std::string& uiFontName,
                  const std::string& monoFontName,
                  const std::vector<float>& sizes,
                  float defaultSize);

        // 取最接近 size 的 UI / Mono 字体；找不到回退到 ImGui 默认。
        ImFont* uiFont(float size) const;
        ImFont* monoFont(float size) const;

        // 当前默认 UI / Mono 字体。
        ImFont* defaultUi() const { return m_defaultUi; }
        ImFont* defaultMono() const { return m_defaultMono; }

    private:
        struct Entry
        {
            float size;
            ImFont* font;
        };

        // 按 size 找最接近的字体。
        ImFont* pickClosest(const std::vector<Entry>& list, float size) const;

        std::vector<Entry> m_uiFonts;
        std::vector<Entry> m_monoFonts;
        ImFont* m_defaultUi = nullptr;
        ImFont* m_defaultMono = nullptr;
    };
}

#endif
