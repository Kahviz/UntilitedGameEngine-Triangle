#include "App.h"
#include <thread>
#include <chrono>
#include "Graphics.h"
#include <vector>
#include "Mesh.h"
#include <DirectXMath.h>
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

int screen_width = GetSystemMetrics(SM_CXSCREEN);
int screen_height = GetSystemMetrics(SM_CYSCREEN);

using namespace DirectX;

auto const NAME = "Untitled GameEngine";

struct Object
{
    Mesh mesh;
    XMFLOAT3 pos;
    XMFLOAT3 Velocity;
    XMFLOAT3 Orientation;
    XMINT3 color;
    bool Anchored;
    std::string Name;
};

std::vector<Object> Drawables;

App::App()
    : wnd(screen_width,screen_height, NAME,true)
{
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
}

int App::Go()
{
    using clock = std::chrono::high_resolution_clock;
    auto last = clock::now();

    while (true)
    {
        if (const auto ecode = Window::ProcessMessages())
        {
            return *ecode;
        }

        auto now = clock::now();
        std::chrono::duration<float> delta = now - last;
        last = now;

        DoFrame(delta.count());
    }
}
void App::AddAMesh(std::string Path,std::string Name,bool Anchored,XMFLOAT3 pos, XMFLOAT3 vel, XMFLOAT3 Orientation,XMINT3 color)
{
    Drawables.emplace_back();
    auto& obj = Drawables.back();

    obj.mesh.Load(Path, wnd.Gfx().GetDevice());
    obj.Name = Name;
    obj.Anchored = Anchored;
    obj.pos = pos;
    obj.Velocity = vel;
    obj.Orientation = Orientation;
    obj.color = color;
}
void App::DoFrame(float deltaTime)
{
    wnd.Gfx().ClearBuffer(0.0f, 0.0f, 0.1f);

    camC.MakeCameraControls(wnd, deltaTime);

    for (auto& obj : Drawables)
    {
        wnd.Gfx().DrawMesh(deltaTime, obj.mesh, obj.Orientation, obj.pos, obj.color, obj.Velocity, obj.Anchored);
    }

    wnd.mouse.Update();

    SetWindowTextW(wnd.hWnd, std::to_wstring(1.0f / deltaTime).c_str());

    if (wnd.kbd.KeyIsPressed(VK_CONTROL) && wnd.kbd.KeyIsPressed(0x35))
    {
        Drawables.emplace_back();
        auto& obj = Drawables.back();

        obj.mesh.Load("C:\\Program Files\\UntilitedGameEngine\\Assets\\UntitledSUS.fbx", wnd.Gfx().GetDevice());
        obj.Name = "Sus";
        obj.Anchored = false;
        obj.pos = { 0.0f, 5.0f, 0.0f };
        obj.Velocity = { 1.0f, 0.0f, 0.0f };
        obj.Orientation = { 0.0f, timer.Peek(), timer.Peek() };
        obj.color = { 10, 10, 0 };
    }

    // Aloita frame vain kerran
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // Explorer (oikea reuna)
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

    //AllTools
    if (ImGui::Button(("Add"), ImVec2(screen_width / 17, screen_height / 10))) {}
    if (ImGui::IsItemHovered())
    {
        ImGui::OpenPopup("AddOptionsPopup");
    }

    // Popup-ikkuna
    if (ImGui::BeginPopup("AddOptionsPopup"))
    {
        if (ImGui::Button("Cube")) { /* action */ }
        if (ImGui::Button("Plane")) { /* action */ }
        if (ImGui::Button("Ball")) { /* action */ }
        if (ImGui::Button("Cylinder")) { /* action */ }
        if (ImGui::Button("Dont Do it")) { /* action */ }
        ImGui::EndPopup();
    }

    ImGui::End();

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    wnd.Gfx().EndFrame();

    if (wnd.kbd.KeyIsPressed(VK_TAB))
    {
        std::exit(-25);
    }

}
