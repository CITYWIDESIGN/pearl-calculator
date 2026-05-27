#include "FontManager.h"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <string>

namespace pearl::gui
{
    namespace
    {
        // 在 Windows 字体目录里寻找候选字体文件。无匹配时返回空串。
        std::string locateWindowsFont(const std::string& family)
        {
            namespace fs = std::filesystem;
            const std::vector<std::string> winDirs = {
                "C:/Windows/Fonts",
                "C:/Windows/Fonts/"
            };
            // 常见 family -> 文件名候选
            std::vector<std::string> candidates;
            const std::string lower = [&] {
                std::string s = family;
                std::transform(s.begin(), s.end(), s.begin(), ::tolower);
                return s;
            }();

            if (lower.find("yahei") != std::string::npos || lower.find("雅黑") != std::string::npos)
            {
                candidates = {"msyh.ttc", "msyh.ttf", "msyhbd.ttc", "msyhl.ttc"};
            }
            else if (lower.find("jetbrains") != std::string::npos)
            {
                candidates = {
                    "JetBrainsMono-Regular.ttf",
                    "JetBrainsMono[wght].ttf",
                    "JetBrainsMonoNL-Regular.ttf"
                };
            }
            else if (lower.find("consolas") != std::string::npos)
            {
                candidates = {"consola.ttf"};
            }
            else if (lower.find("segoe") != std::string::npos)
            {
                candidates = {"segoeui.ttf"};
            }
            else
            {
                candidates = {family + ".ttf", family + ".ttc"};
            }

            for (const auto& dir : winDirs)
            {
                for (const auto& name : candidates)
                {
                    fs::path p = fs::path(dir) / name;
                    std::error_code ec;
                    if (fs::exists(p, ec)) return p.string();
                }
            }

            // 通用 fallback：mono 用 consolas，UI 用 segoeui
            std::error_code ec;
            if (lower.find("mono") != std::string::npos || lower.find("jetbrains") != std::string::npos)
            {
                fs::path fb = "C:/Windows/Fonts/consola.ttf";
                if (fs::exists(fb, ec)) return fb.string();
            }
            fs::path fb = "C:/Windows/Fonts/segoeui.ttf";
            if (fs::exists(fb, ec)) return fb.string();
            return {};
        }
    }

    void FontManager::load(const std::string& uiFontName,
                           const std::string& monoFontName,
                           const std::vector<float>& sizes,
                           float defaultSize)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->Clear();

        const std::string uiPath = locateWindowsFont(uiFontName);
        const std::string monoPath = locateWindowsFont(monoFontName);

        const ImWchar* ranges = io.Fonts->GetGlyphRangesChineseSimplifiedCommon();

        m_uiFonts.clear();
        m_monoFonts.clear();
        m_defaultUi = nullptr;
        m_defaultMono = nullptr;

        // 中文 glyph 集合很大，每个尺寸加载一份会显著占内存且拖慢启动。
        // 只加载默认尺寸 + 一档 small + 一档 large；其他尺寸由 SetWindowFontScale 处理。
        std::vector<float> wanted;
        wanted.push_back(defaultSize);
        for (float s : sizes)
        {
            if (std::abs(s - defaultSize) > 0.5f && wanted.size() < 3)
            {
                bool dup = false;
                for (float w : wanted) if (std::abs(w - s) < 0.5f) { dup = true; break; }
                if (!dup) wanted.push_back(s);
            }
        }

        for (float size : wanted)
        {
            ImFontConfig cfg;
            cfg.OversampleH = 2;
            cfg.OversampleV = 1;
            cfg.PixelSnapH  = true;

            ImFont* ui = nullptr;
            if (!uiPath.empty())
            {
                ui = io.Fonts->AddFontFromFileTTF(uiPath.c_str(), size, &cfg, ranges);
            }
            if (!ui)
            {
                ImFontConfig def;
                def.SizePixels = size;
                ui = io.Fonts->AddFontDefault(&def);
            }
            m_uiFonts.push_back({size, ui});

            ImFont* mono = nullptr;
            if (!monoPath.empty())
            {
                ImFontConfig mcfg;
                mcfg.OversampleH = 2;
                mcfg.OversampleV = 1;
                mcfg.PixelSnapH  = true;
                mono = io.Fonts->AddFontFromFileTTF(monoPath.c_str(), size, &mcfg);
            }
            if (!mono)
            {
                ImFontConfig def;
                def.SizePixels = size;
                mono = io.Fonts->AddFontDefault(&def);
            }
            // 等宽字体（JetBrains / Consolas）不含中文字形 → 控制台显示为方框 / 乱码。
            // 把 UI 字体的中文字形合并到等宽字体上，作为缺失字符的后备。
            if (mono && !uiPath.empty())
            {
                ImFontConfig merge;
                merge.MergeMode    = true;
                merge.OversampleH  = 1;
                merge.OversampleV  = 1;
                merge.PixelSnapH   = true;
                merge.GlyphMinAdvanceX = size; // 让中文也按等宽对齐
                io.Fonts->AddFontFromFileTTF(uiPath.c_str(), size, &merge, ranges);
            }
            m_monoFonts.push_back({size, mono});
        }

        m_defaultUi = pickClosest(m_uiFonts, defaultSize);
        m_defaultMono = pickClosest(m_monoFonts, defaultSize);

        io.FontDefault = m_defaultUi;
        // 不调用 io.Fonts->Build()：后端 Init/NewFrame 会按需重建 atlas。
    }

    ImFont* FontManager::pickClosest(const std::vector<Entry>& list, float size) const
    {
        if (list.empty()) return nullptr;
        ImFont* best = list.front().font;
        float bestDiff = std::abs(list.front().size - size);
        for (const auto& e : list)
        {
            const float d = std::abs(e.size - size);
            if (d < bestDiff)
            {
                bestDiff = d;
                best = e.font;
            }
        }
        return best;
    }

    ImFont* FontManager::uiFont(float size) const
    {
        ImFont* f = pickClosest(m_uiFonts, size);
        return f ? f : m_defaultUi;
    }

    ImFont* FontManager::monoFont(float size) const
    {
        ImFont* f = pickClosest(m_monoFonts, size);
        return f ? f : m_defaultMono;
    }
}
