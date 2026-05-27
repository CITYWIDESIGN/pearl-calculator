#include "CalculatorWindow.h"

#include <algorithm>
#include <cstring>
#include <string>
#include <vector>

#include <imgui.h>
#include <imgui_internal.h>

#include "../animation/Animator.h"
#include "../animation/Easing.h"
#include "../app/AppContext.h"
#include "../i18n/I18n.h"
#include "../theme/Theme.h"
#include "../widgets/AnimatedTabBar.h"
#include "../widgets/Card.h"

namespace pearl::gui
{
    namespace
    {
        // 标签 + 输入框：上下两行布局，整齐美观。
        bool StackedInput(const char* label, std::string& buffer, bool disabled = false)
        {
            const auto& p = currentPalette();
            ImGui::PushID(label);
            ImGui::PushStyleColor(ImGuiCol_Text, p.textMuted);
            ImGui::TextUnformatted(label);
            ImGui::PopStyleColor();

            char data[64] = {};
            std::strncpy(data, buffer.c_str(), sizeof(data) - 1);
            ImGui::PushItemWidth(-FLT_MIN);
            const ImGuiInputTextFlags flags = disabled ? ImGuiInputTextFlags_ReadOnly : 0;
            const bool changed = ImGui::InputText("##v", data, sizeof(data), flags);
            ImGui::PopItemWidth();
            if (changed && !disabled) buffer = data;
            ImGui::PopID();
            return changed;
        }

        bool DirectionCombo(const char* label, std::string& dir, Animator& animator)
        {
            const auto& p = currentPalette();
            ImGui::PushID(label);
            ImGui::PushStyleColor(ImGuiCol_Text, p.textMuted);
            ImGui::TextUnformatted(label);
            ImGui::PopStyleColor();

            const char* dirs[] = {"North", "South", "East", "West"};
            int idx = 0;
            for (int i = 0; i < 4; ++i) if (dir == dirs[i]) idx = i;
            ImGui::PushItemWidth(-FLT_MIN);
            const bool changed = widgets::AnimatedCombo("dir", dirs, 4, &idx, animator);
            ImGui::PopItemWidth();
            if (changed) dir = dirs[idx];
            ImGui::PopID();
            return changed;
        }

        // 主操作按钮：用 accent 色填充。
        bool PrimaryButton(const char* label, const ImVec2& size)
        {
            const auto& p = currentPalette();
            ImGui::PushStyleColor(ImGuiCol_Button, p.accent);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, p.accentHover);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, p.accentActive);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
            const bool clicked = ImGui::Button(label, size);
            ImGui::PopStyleColor(4);
            return clicked;
        }

        // 左侧：坐标录入卡片。
        void renderInputPanel(AppContext& ctx, float width)
        {
            ImGui::BeginChild("input_panel", ImVec2(width, 0), false);

            widgets::BeginCard("input_card");
            {
                widgets::CardHeader(t("calc.input"), t("calc.input_sub"));

                auto& form = ctx.form();

                if (ImGui::Checkbox(t("calc.use_y"), &form.useY))
                {
                    // no-op
                }

                ImGui::Dummy(ImVec2(0, 4));

                StackedInput(t("calc.target_x"), form.targetX);
                ImGui::Dummy(ImVec2(0, 6));

                // Y 坐标输入：alpha + 微位移 (translateY) 双轨缓动。
                // 高度槽位用一个独立 tween，但只取末态时调整 Dummy，避免 BeginChild
                // 每帧改变内容 → 父布局 reflow → 卡顿。
                const float yRowFullH = 56.0f;
                Animator& anim = ctx.animator();
                const float yA = static_cast<float>(anim.driveTo(
                    "calc_y_alpha", form.useY ? 1.0 : 0.0,
                    0.28, Easing::OutCubic, form.useY ? 1.0 : 0.0));
                const float yH = static_cast<float>(anim.driveTo(
                    "calc_y_height", form.useY ? yRowFullH : 0.0,
                    0.28, Easing::OutCubic, form.useY ? yRowFullH : 0.0));

                if (yH > 0.5f)
                {
                    // 用 clip rect + 屏幕坐标位移渲染 StackedInput，对父级 layout
                    // 而言只占用一个 Dummy 高度 = yH，不会触发其他控件位置抖动。
                    const ImVec2 slotStart = ImGui::GetCursorScreenPos();
                    const float slotW = ImGui::GetContentRegionAvail().x;
                    const ImVec2 slotEnd(slotStart.x + slotW, slotStart.y + yH);

                    // 横向位移：从 -slotW（左侧屏幕外）滑入到 0；收起时反向滑出
                    const float translateX = (yA - 1.0f) * slotW;

                    ImGui::PushClipRect(slotStart, slotEnd, true);
                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, yA);

                    // 把内部内容画在 slotStart.x + translateX 的位置上
                    ImGui::SetCursorScreenPos(ImVec2(slotStart.x + translateX, slotStart.y));
                    StackedInput(t("calc.target_y"), form.targetY);

                    ImGui::PopStyleVar();
                    ImGui::PopClipRect();

                    // 占位：让后续控件知道这段槽位实际占用了 yH（动画过程中槽位不变）
                    ImGui::SetCursorScreenPos(slotEnd);
                    ImGui::Dummy(ImVec2(0, 6));
                }

                StackedInput(t("calc.target_z"), form.targetZ);

                widgets::SoftDivider(8.0f);

                StackedInput(t("calc.red_tnt"), form.redTnt);
                ImGui::Dummy(ImVec2(0, 6));

                StackedInput(t("calc.blue_tnt"), form.blueTnt);
                ImGui::Dummy(ImVec2(0, 6));

                DirectionCombo(t("calc.direction"), form.direction, ctx.animator());

                widgets::SoftDivider(10.0f);

                // 三个操作按钮高度统一为 34；主操作 (Solve) 用 accent 色
                const float actionsW = ImGui::GetContentRegionAvail().x;
                const float btnH = 34.0f;
                if (PrimaryButton(t("calc.btn.solve"), ImVec2(actionsW, btnH)))
                    ctx.runSolve();

                ImGui::Dummy(ImVec2(0, 6));
                if (ImGui::Button(t("calc.btn.simulate"), ImVec2(actionsW, btnH)))
                    ctx.runSimulationFromForm();

                ImGui::Dummy(ImVec2(0, 6));
                if (ImGui::Button(t("calc.btn.reset"), ImVec2(actionsW, btnH)))
                    ctx.resetForm();
            }
            widgets::EndCard();

            ImGui::EndChild();
        }

        // ----- 伪表格基础设施 -----
        struct PseudoColumn
        {
            const char* label;
            float weight;         // > 0：弹性权重；按总可用宽度的相对比例分配
            float minWidth;       // 最小宽度（保护数字 / 标识列不被挤瘪）
            float align;          // 0=left, 0.5=center, 1=right
        };

        // 绘制表头条；返回每列的左边界 x 坐标列表（含右端，长度 = cols.size()+1）。
        std::vector<float> drawPseudoHeader(const std::vector<PseudoColumn>& cols,
                                            float rowH)
        {
            const auto& p = currentPalette();
            ImDrawList* dl = ImGui::GetWindowDrawList();
            const ImVec2 origin = ImGui::GetCursorScreenPos();
            const float availW = ImGui::GetContentRegionAvail().x;

            // 1) 先派 minWidth；2) 把剩余按 weight 分配。
            float weightSum = 0.0f;
            float minSum = 0.0f;
            for (const auto& c : cols) { weightSum += c.weight; minSum += c.minWidth; }
            const float remaining = std::max(0.0f, availW - minSum);

            std::vector<float> widths(cols.size(), 0.0f);
            for (size_t i = 0; i < cols.size(); ++i)
            {
                const float share = (weightSum > 0.0f)
                    ? remaining * (cols[i].weight / weightSum) : 0.0f;
                widths[i] = cols[i].minWidth + share;
            }

            std::vector<float> xs;
            xs.reserve(cols.size() + 1);
            float x = origin.x;
            xs.push_back(x);
            for (float w : widths) { x += w; xs.push_back(x); }

            // 表头背景 + 下边线
            dl->AddRectFilled(origin,
                              ImVec2(origin.x + availW, origin.y + rowH),
                              ImGui::ColorConvertFloat4ToU32(p.surfaceElevated),
                              ImGui::GetStyle().FrameRounding);
            dl->AddLine(ImVec2(origin.x, origin.y + rowH - 0.5f),
                        ImVec2(origin.x + availW, origin.y + rowH - 0.5f),
                        ImGui::ColorConvertFloat4ToU32(p.border), 1.0f);

            // 表头文本（按列 align 对齐 + 单行截断）
            for (size_t i = 0; i < cols.size(); ++i)
            {
                const char* lbl = cols[i].label;
                const float colL = xs[i] + 8.0f;
                const float colR = xs[i + 1] - 8.0f;
                const ImVec2 ts = ImGui::CalcTextSize(lbl);
                const float tx = colL + (colR - colL - ts.x) * cols[i].align;
                dl->PushClipRect(ImVec2(colL, origin.y),
                                 ImVec2(colR, origin.y + rowH), true);
                dl->AddText(ImVec2(tx, origin.y + (rowH - ts.y) * 0.5f),
                            ImGui::ColorConvertFloat4ToU32(p.textSecondary), lbl);
                dl->PopClipRect();
            }

            ImGui::Dummy(ImVec2(availW, rowH));
            return xs;
        }

        // 计算单行的 (alpha, translateY) 入场动效参数。
        struct RowAnim { float alpha; float dy; };
        RowAnim computeRowAnim(double secondsSinceGen, int rowIndex)
        {
            // 整体节奏比之前快 1.5×：阶梯延迟 27ms / 行，单行 280ms
            constexpr double perRow   = 0.027;
            constexpr double duration = 0.28;
            const double localT = secondsSinceGen - rowIndex * perRow;
            if (localT <= 0.0) return {0.0f, 15.0f};
            if (localT >= duration) return {1.0f, 0.0f};
            const double tNorm = localT / duration;
            const double eased = easing::easeOutCubic(tNorm);
            return { static_cast<float>(eased),
                     static_cast<float>((1.0 - eased) * 15.0) };
        }

        void renderPlansList(AppContext& ctx)
        {
            const auto& plans = ctx.plans();
            if (plans.empty())
            {
                ImGui::PushStyleColor(ImGuiCol_Text, currentPalette().textMuted);
                ImGui::TextWrapped("%s", t("calc.empty.plans"));
                ImGui::PopStyleColor();
                return;
            }

            const std::vector<PseudoColumn> cols = {
                // # + 偏移 共占 10%（3 + 7）；其余 4 列各 22.5%（总 90%）
                // label,                 weight, minW, align
                { t("calc.col.idx"),     3.0f,  28.0f, 0.5f },  // # 列：居中
                { t("calc.col.offset"),  7.0f,  56.0f, 1.0f },  // 偏移：右对齐（数值）
                { t("calc.col.tick"),   22.5f,  56.0f, 1.0f },
                { t("calc.col.red"),    22.5f,  56.0f, 1.0f },
                { t("calc.col.blue"),   22.5f,  56.0f, 1.0f },
                { t("calc.col.dir"),    22.5f,  64.0f, 0.5f },  // 方向：居中
            };

            const float headerH = 28.0f;
            const float rowH    = 26.0f;
            const auto& palette = currentPalette();
            const double seconds = ctx.secondsSincePlansGen();

            const std::vector<float> xs = drawPseudoHeader(cols, headerH);

            ImGui::BeginChild("plans_scroll", ImVec2(0, 0), false);
            ImDrawList* dl = ImGui::GetWindowDrawList();

            for (int i = 0; i < static_cast<int>(plans.size()); ++i)
            {
                const auto& plan = plans[i];
                const RowAnim ra = computeRowAnim(seconds, i);

                // 行槽位：先放一个 InvisibleButton 占满 rowH，确保父容器边界正确扩展。
                ImGui::PushID(i);
                const ImVec2 rowOrigin = ImGui::GetCursorScreenPos();
                const float availW = ImGui::GetContentRegionAvail().x;
                ImGui::InvisibleButton("row", ImVec2(availW, rowH));
                const bool hovered = ImGui::IsItemHovered();
                const bool clicked = ImGui::IsItemClicked();
                ImGui::PopID();

                if (ra.alpha <= 0.001f) continue;

                // 视觉上把内容向上微移 ra.dy（相对于槽位顶部）
                const ImVec2 rowMin(rowOrigin.x, rowOrigin.y + ra.dy);
                const ImVec2 rowMax(rowOrigin.x + availW, rowMin.y + rowH);

                const bool selected = ctx.selectedPlanIndex().has_value()
                                   && *ctx.selectedPlanIndex() == i;

                ImVec4 bg = (i % 2 == 0) ? ImVec4(0,0,0,0)
                                          : ImVec4(palette.surface.x, palette.surface.y, palette.surface.z, 0.35f);
                if (hovered)  bg = ImVec4(palette.accent.x, palette.accent.y, palette.accent.z, 0.12f);
                if (selected) bg = ImVec4(palette.accent.x, palette.accent.y, palette.accent.z, 0.22f);
                bg.w *= ra.alpha;
                dl->AddRectFilled(rowMin, rowMax, ImGui::ColorConvertFloat4ToU32(bg));

                if (clicked) ctx.selectPlan(i);

                char buf[64];
                auto drawCell = [&](int colIdx, const char* text)
                {
                    const float cl = xs[colIdx] + 8.0f;
                    const float cr = xs[colIdx + 1] - 8.0f;
                    const ImVec2 ts = ImGui::CalcTextSize(text);
                    const float tx = cl + (cr - cl - ts.x) * cols[colIdx].align;
                    ImVec4 col = palette.textPrimary;
                    col.w *= ra.alpha;
                    dl->AddText(ImVec2(tx, rowMin.y + (rowH - ts.y) * 0.5f),
                                ImGui::ColorConvertFloat4ToU32(col), text);
                };

                std::snprintf(buf, sizeof(buf), "%d", i + 1);              drawCell(0, buf);
                std::snprintf(buf, sizeof(buf), "%.5f", plan.landingOffset);drawCell(1, buf);
                std::snprintf(buf, sizeof(buf), "%d", plan.arrivalGameTick);drawCell(2, buf);
                std::snprintf(buf, sizeof(buf), "%d", plan.configuration.redTnt);  drawCell(3, buf);
                std::snprintf(buf, sizeof(buf), "%d", plan.configuration.blueTnt); drawCell(4, buf);
                drawCell(5, plan.configuration.direction.c_str());

                dl->AddLine(ImVec2(rowMin.x, rowMax.y - 0.5f),
                            ImVec2(rowMax.x, rowMax.y - 0.5f),
                            ImGui::ColorConvertFloat4ToU32(
                                ImVec4(palette.divider.x, palette.divider.y, palette.divider.z,
                                       palette.divider.w * ra.alpha)),
                            1.0f);
            }

            ImGui::EndChild();
        }

        void renderTrajectoryList(AppContext& ctx)
        {
            const auto& tr = ctx.trajectory();
            if (tr.empty())
            {
                ImGui::PushStyleColor(ImGuiCol_Text, currentPalette().textMuted);
                ImGui::TextWrapped("%s", t("calc.empty.traj"));
                ImGui::PopStyleColor();
                return;
            }

            const std::vector<PseudoColumn> cols = {
                // 游戏刻占 10%；其余 6 列各 15%（合计 90%）
                { t("calc.col.tick"), 10.0f, 56.0f, 0.5f },
                { "X",  15.0f, 64.0f, 1.0f },
                { "Y",  15.0f, 64.0f, 1.0f },
                { "Z",  15.0f, 64.0f, 1.0f },
                { "MX", 15.0f, 56.0f, 1.0f },
                { "MY", 15.0f, 56.0f, 1.0f },
                { "MZ", 15.0f, 56.0f, 1.0f },
            };

            const float headerH = 28.0f;
            const float rowH    = 26.0f; // 与 plans 一致
            const auto& palette = currentPalette();
            const double seconds = ctx.secondsSinceTrajGen();

            const std::vector<float> xs = drawPseudoHeader(cols, headerH);

            ImGui::BeginChild("traj_scroll", ImVec2(0, 0), false);
            ImDrawList* dl = ImGui::GetWindowDrawList();

            ImGuiListClipper clipper;
            clipper.Begin(static_cast<int>(tr.size()), rowH);
            while (clipper.Step())
            {
                for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
                {
                    const auto& pt = tr[i];
                    const RowAnim ra = computeRowAnim(seconds, i);

                    ImGui::PushID(i);
                    const ImVec2 rowOrigin = ImGui::GetCursorScreenPos();
                    const float availW = ImGui::GetContentRegionAvail().x;
                    ImGui::InvisibleButton("row", ImVec2(availW, rowH),
                                           ImGuiButtonFlags_MouseButtonLeft
                                         | ImGuiButtonFlags_MouseButtonRight);
                    const bool hovered = ImGui::IsItemHovered();
                    const bool leftClicked  = ImGui::IsItemClicked(ImGuiMouseButton_Left);
                    const bool rightClicked = ImGui::IsItemClicked(ImGuiMouseButton_Right);

                    if (leftClicked || rightClicked) ctx.selectedTrajIndex() = i;

                    if (rightClicked) ImGui::OpenPopup("traj_ctx");

                    if (ImGui::BeginPopup("traj_ctx"))
                    {
                        if (ImGui::MenuItem(t("calc.ctx.copy_tp")))
                        {
                            char tp[128];
                            std::snprintf(tp, sizeof(tp),
                                          "/tp @s %.5f %.5f %.5f",
                                          pt.position.x, pt.position.y, pt.position.z);
                            ImGui::SetClipboardText(tp);
                            ctx.emitLog(t("toast.tp_copied"), widgets::ToastKind::Success);
                        }
                        ImGui::EndPopup();
                    }
                    ImGui::PopID();

                    if (ra.alpha <= 0.001f) continue;

                    const bool selected = ctx.selectedTrajIndex().has_value()
                                       && *ctx.selectedTrajIndex() == i;

                    const ImVec2 rowMin(rowOrigin.x, rowOrigin.y + ra.dy);
                    const ImVec2 rowMax(rowOrigin.x + availW, rowMin.y + rowH);

                    // 与 plans 一致的行背景：斑马 + hover + selected
                    ImVec4 bg = (i % 2 == 0)
                        ? ImVec4(0,0,0,0)
                        : ImVec4(palette.surface.x, palette.surface.y, palette.surface.z, 0.35f);
                    if (hovered)  bg = ImVec4(palette.accent.x, palette.accent.y, palette.accent.z, 0.12f);
                    if (selected) bg = ImVec4(palette.accent.x, palette.accent.y, palette.accent.z, 0.22f);
                    bg.w *= ra.alpha;
                    dl->AddRectFilled(rowMin, rowMax, ImGui::ColorConvertFloat4ToU32(bg));

                    char buf[64];
                    auto drawCell = [&](int colIdx, const char* text)
                    {
                        const float cl = xs[colIdx] + 8.0f;
                        const float cr = xs[colIdx + 1] - 8.0f;
                        const ImVec2 ts = ImGui::CalcTextSize(text);
                        const float tx = cl + (cr - cl - ts.x) * cols[colIdx].align;
                        ImVec4 col = palette.textPrimary;
                        col.w *= ra.alpha;
                        dl->AddText(ImVec2(tx, rowMin.y + (rowH - ts.y) * 0.5f),
                                    ImGui::ColorConvertFloat4ToU32(col), text);
                    };

                    std::snprintf(buf, sizeof(buf), "%d",   pt.gameTick);   drawCell(0, buf);
                    std::snprintf(buf, sizeof(buf), "%.5f", pt.position.x); drawCell(1, buf);
                    std::snprintf(buf, sizeof(buf), "%.5f", pt.position.y); drawCell(2, buf);
                    std::snprintf(buf, sizeof(buf), "%.5f", pt.position.z); drawCell(3, buf);
                    std::snprintf(buf, sizeof(buf), "%.5f", pt.motion.x);   drawCell(4, buf);
                    std::snprintf(buf, sizeof(buf), "%.5f", pt.motion.y);   drawCell(5, buf);
                    std::snprintf(buf, sizeof(buf), "%.5f", pt.motion.z);   drawCell(6, buf);

                    // 分隔细线
                    dl->AddLine(ImVec2(rowMin.x, rowMax.y - 0.5f),
                                ImVec2(rowMax.x, rowMax.y - 0.5f),
                                ImGui::ColorConvertFloat4ToU32(
                                    ImVec4(palette.divider.x, palette.divider.y, palette.divider.z,
                                           palette.divider.w * ra.alpha)),
                                1.0f);
                }
            }
            clipper.End();

            ImGui::EndChild();
        }

        void renderResultPanel(AppContext& ctx)
        {
            ImGui::BeginChild("result_panel", ImVec2(0, 0), false);

            widgets::BeginCard("result_card", ImVec2(0, 0));
            {
                const std::vector<const char*> tabs = {t("calc.tab.plans"), t("calc.tab.trajectory")};
                int active = static_cast<int>(ctx.rightPanel());
                const int next = widgets::AnimatedTabBar("right_tabs", tabs, active, ctx.animator());
                if (next != active)
                {
                    ctx.rightPanel() = static_cast<RightPanelMode>(next);
                    // 切换 Tab 时重置入场计时器，让伪表格的级联浮现重新播一遍
                    if (ctx.rightPanel() == RightPanelMode::Plans)
                        ctx.resetPlansGenTimer();
                    else
                        ctx.resetTrajGenTimer();
                }

                ImGui::BeginChild("right_body", ImVec2(0, 0), false);
                if (ctx.rightPanel() == RightPanelMode::Plans)
                    renderPlansList(ctx);
                else
                    renderTrajectoryList(ctx);
                ImGui::EndChild();
            }
            widgets::EndCard();

            ImGui::EndChild();
        }
    }

    void CalculatorWindow::render(bool* /*open*/)
    {
        ImGuiViewport* vp = ImGui::GetMainViewport();
        const float titleH = 36.0f;
        ImGui::SetNextWindowPos(ImVec2(vp->Pos.x, vp->Pos.y + titleH));
        ImGui::SetNextWindowSize(ImVec2(vp->Size.x, vp->Size.y - titleH));

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 14));

        const ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse
                                     | ImGuiWindowFlags_NoMove
                                     | ImGuiWindowFlags_NoResize
                                     | ImGuiWindowFlags_NoTitleBar
                                     | ImGuiWindowFlags_NoBringToFrontOnFocus
                                     | ImGuiWindowFlags_NoSavedSettings;

        if (ImGui::Begin("##pearl_main", nullptr, flags))
        {
            const float availW = ImGui::GetContentRegionAvail().x;
            const float leftW  = std::clamp(availW * 0.30f, 280.0f, 360.0f);

            renderInputPanel(m_ctx, leftW);
            ImGui::SameLine();
            renderResultPanel(m_ctx);
        }
        ImGui::End();

        ImGui::PopStyleVar(3);
    }
}
