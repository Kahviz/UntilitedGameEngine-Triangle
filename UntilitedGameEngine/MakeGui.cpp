#include "MakeGui.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#include <Windows.h>

struct MeshButton
{
    const char* label;
    std::string path; // EI osoitinta
    const char* name;
};

static MeshButton meshButtons[] =
{
    { "Cube",     assets + "\\Cube.fbx",     "Cube" },
    { "Plane",    assets + "\\Plane.fbx",    "Plane" },
    { "Ball",     assets + "\\BallSUS.fbx",  "Ball" },
    { "Cylinder", assets + "\\Cylinder.fbx", "Cylinder"}
};

void MakeGui::MakeIMGui(
    const std::vector<Object>& Drawables,
    std::function<void(const std::string&, const std::string&, DirectX::XMFLOAT3)> AddAMesh,
    float* Color3)
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // Explorer
    ImGui::SetNextWindowPos(ImVec2(screen_width - screen_width / 5, screen_height / 5));
    ImGui::SetNextWindowSize(ImVec2(screen_width / 5, screen_height - screen_height / 5));

    ImGui::Begin("Explorer", nullptr,
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_HorizontalScrollbar);

    for (const Object& obj : Drawables)
    {
        ImGui::Text("%s", obj.Name.c_str());
    }

    ImGui::End();

    // HomeTab
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(screen_width, screen_height / 5));

    ImGui::Begin("HomeTab", nullptr,
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse
    );

    // Add button
    if (ImGui::Button("Add", ImVec2(screen_width / 17.0f, screen_height / 10.0f))) {}
    if (ImGui::IsItemHovered())
        ImGui::OpenPopup("AddOptionsPopup");

    ImGui::SameLine();

    // Color picker button
    if (ImGui::ColorButton("Color", ImVec4(Color3[0], Color3[1], Color3[2], Color3[3]),
        0, ImVec2(screen_width / 17.0f, screen_height / 10.0f)))
    {
        ImGui::OpenPopup("ColorPickerPopup");
    }

    ImGui::SetNextWindowSize(ImVec2(screen_width / 8.5f, screen_height / 5));

    if (ImGui::BeginPopup("ColorPickerPopup"))
    {
        ImGui::ColorPicker4("##picker", Color3,
            ImGuiColorEditFlags_PickerHueWheel |
            ImGuiColorEditFlags_NoInputs |
            ImGuiColorEditFlags_NoAlpha);
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopup("AddOptionsPopup"))
    {
        for (const auto& mesh : meshButtons)
        {
            if (ImGui::Button(mesh.label))
            {
                AddAMesh(mesh.path, mesh.name, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
            }
        }
        ImGui::EndPopup();
    }

    ImGui::End();

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
