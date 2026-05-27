#include "I18n.h"

#include <string_view>
#include <unordered_map>

namespace pearl::gui
{
    namespace
    {
        Language g_lang = Language::ChineseSimp;

        struct Entry { const char* en; const char* zh; };

        // 集中管理所有界面字串。新增字串时在这里加一行即可。
        const std::unordered_map<std::string_view, Entry>& dict()
        {
            static const std::unordered_map<std::string_view, Entry> m = {
                // 标题栏菜单
                {"menu.file",          {"File",       "文件"}},
                {"menu.view",          {"View",       "视图"}},
                {"menu.help",          {"Help",       "帮助"}},
                {"menu.load_config",   {"Load Config...", "加载配置文件..."}},
                {"menu.settings",      {"Settings",   "设置"}},
                {"menu.console",       {"Console",    "控制台"}},
                {"menu.about",         {"About",      "关于"}},

                // 状态
                {"toast.config_loaded",{"Config loaded.","配置文件已加载。"}},
                {"toast.config_cancel",{"Config load cancelled.","已取消加载配置。"}},

                // 设置窗口
                {"settings.title",     {"Settings",   "设置"}},
                {"settings.tab.menu",  {"UI",         "菜单设置"}},
                {"settings.tab.calc",  {"Compute",    "计算设置"}},
                {"settings.section.appearance",  {"Appearance",   "外观"}},
                {"settings.section.appearance_sub", {"theme / radius / opacity", "主题 / 圆角 / 透明度"}},
                {"settings.theme",     {"Theme",      "主题"}},
                {"settings.theme.dark",{"Dark",       "深色"}},
                {"settings.theme.light",{"Light",     "亮色"}},
                {"settings.viewport",  {"Viewport",   "视口模式"}},
                {"settings.viewport.2d",{"2D Top",    "2D 俯视"}},
                {"settings.viewport.3d",{"3D",        "3D 透视"}},
                {"settings.corner",    {"Corner Radius", "圆角"}},
                {"settings.opacity",   {"Window Opacity","窗口不透明度"}},
                {"settings.section.fonts", {"Fonts",  "字体"}},
                {"settings.section.fonts_sub", {"UI / numeric", "UI / 数值"}},
                {"settings.font.ui",   {"UI Font",    "UI 字体"}},
                {"settings.font.mono", {"Monospace",  "等宽字体"}},
                {"settings.font.size", {"Font Size",  "字体大小"}},
                {"settings.ui_scale",  {"UI Scale",   "UI 缩放"}},
                {"settings.font.note", {"Restart GUI to reload fonts.","修改字体后，需要重启 GUI 才会重新加载。"}},
                {"settings.section.motion", {"Motion","动效"}},
                {"settings.section.motion_sub", {"animation / layout","动画速度 / 布局"}},
                {"settings.anim_speed",{"Animation Speed", "动画速度"}},
                {"settings.save_layout",{"Save layout on exit", "退出时保存窗口布局"}},
                {"settings.save_btn",  {"Save",       "保存"}},
                {"settings.section.engine", {"Simulation Engine", "模拟引擎"}},
                {"settings.section.engine_sub", {"tick / rules", "tick / 规则"}},
                {"settings.extra_ticks",{"Extra ticks after arrival", "到达后额外模拟 tick"}},
                {"settings.below_1_21_1",{"Use rules from versions before 1.21.1", "按 1.21.1 之前规则计算"}},
                {"settings.accuracy",  {"Accuracy priority", "精度优先"}},
                {"settings.language",  {"Language",   "语言"}},
                {"settings.lang.en",   {"English",    "English"}},
                {"settings.lang.zh",   {"简体中文",   "简体中文"}},

                // 计算窗口
                {"calc.title",         {"Calculator", "计算器"}},
                {"calc.input",         {"Input",      "坐标录入"}},
                {"calc.input_sub",     {"destination & barrel","destination & barrel"}},
                {"calc.use_y",         {"Enable Y precision mode", "启用 Y 坐标精度模式"}},
                {"calc.target_x",      {"Target X",   "目的地 X"}},
                {"calc.target_y",      {"Target Y",   "目的地 Y"}},
                {"calc.target_z",      {"Target Z",   "目的地 Z"}},
                {"calc.red_tnt",       {"Red TNT",    "红色 TNT"}},
                {"calc.blue_tnt",      {"Blue TNT",   "蓝色 TNT"}},
                {"calc.direction",     {"Direction",  "方向"}},
                {"calc.btn.solve",     {"Solve",      "计算方案"}},
                {"calc.btn.simulate",  {"Simulate",   "开始模拟"}},
                {"calc.btn.reset",     {"Reset",      "重置输入"}},
                {"calc.tab.plans",     {"Plans",      "计算结果"}},
                {"calc.tab.trajectory",{"Trajectory", "珍珠模拟"}},
                {"calc.empty.plans",   {"No plans yet. Click Solve to generate TNT configurations.",
                                        "暂无计算结果。点击「计算方案」生成 TNT 配比。"}},
                {"calc.empty.traj",    {"No trajectory yet. Pick a plan, then click Simulate.",
                                        "暂无模拟轨迹。先选择方案并点击「开始模拟」。"}},
                {"calc.col.idx",       {"#",          "#"}},
                {"calc.col.offset",    {"Offset",     "偏移"}},
                {"calc.col.tick",      {"Tick",       "游戏刻"}},
                {"calc.col.red",       {"Red",        "红 TNT"}},
                {"calc.col.blue",      {"Blue",       "蓝 TNT"}},
                {"calc.col.dir",       {"Direction",  "方向"}},
                {"calc.ctx.copy_tp",   {"Copy /tp",   "复制 TP 指令"}},
                {"toast.tp_copied",    {"/tp command copied to clipboard.", "TP 指令已复制到剪贴板。"}},

                // 关于
                {"about.title",        {"About",      "关于"}},
                {"about.body1",        {"Pearl projectile ballistics tool for Minecraft.",
                                        "Minecraft 珍珠炮弹道计算工具。"}},
                {"about.body2",        {"GUI mode: Dear ImGui + GLFW + OpenGL3.",
                                        "GUI 模式基于 Dear ImGui + GLFW + OpenGL3。"}},
                {"about.body3",        {"CLI / TUI: launch with --cli / --tui.",
                                        "CLI / TUI 模式仍可通过命令行参数 --cli / --tui 启动。"}},
                {"about.config_ver",   {"Config version: %s","配置版本：%s"}},

                // 状态 / Toast
                {"status.ready",       {"Ready.",     "准备就绪。"}},
                {"toast.gui_saved",    {"GUI settings saved.","GUI 设置已保存。"}},
                {"toast.calc_saved",   {"Compute settings saved.","计算设置已保存。"}},

                // 控制台
                {"console.title",      {"Console",    "控制台"}},
                {"console.empty",      {"No log entries yet.","暂无日志。"}},
                {"console.clear",      {"Clear",      "清空"}},
            };
            return m;
        }
    }

    void setLanguage(Language lang) { g_lang = lang; }
    Language currentLanguage() { return g_lang; }

    const char* t(const char* key)
    {
        const auto& m = dict();
        auto it = m.find(std::string_view(key));
        if (it == m.end()) return key;
        return g_lang == Language::English ? it->second.en : it->second.zh;
    }
}
