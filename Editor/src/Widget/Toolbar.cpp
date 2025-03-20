#include "Toolbar.h"

#include <Scene/Scene.h>

#include <imgui.h>
#include <imgui_internal.h>

bool CenteredButton(const char* label, float alignment = 0.5f)
{
    ImGuiStyle& style = ImGui::GetStyle();

    float size = ImGui::CalcTextSize(label).x + style.FramePadding.x * 2.0f;
    float avail = ImGui::GetContentRegionAvail().x;

    float off = (avail - size) * alignment;
    if (off > 0.0f)
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

    return ImGui::Button(label);
}

void Toolbar::Draw(Engine::Scene& scene)
{
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_MenuBar;
    ImGui::BeginViewportSideBar("##Toolbar", nullptr, ImGuiDir_Up, ImGui::GetFrameHeight(), flags);

    ImGui::BeginMenuBar();

    if (scene.IsPaused())
    {
        if (CenteredButton("Play"))
        {
            if (onPlayFn)
            {
                onPlayFn();
            }
        }
    }
    else
    {
        if (CenteredButton("Stop"))
        {
            if (onStopFn)
            {
                onStopFn();
            }
        }
    }

    ImGui::EndMenuBar();

    ImGui::End();
}

void Toolbar::OnPlay(std::function<void()> onPlayFn)
{
    this->onPlayFn = onPlayFn;
}

void Toolbar::OnStop(std::function<void()> onStopFn)
{
    this->onStopFn = onStopFn;
}
