#include "App.h"
#include "Graphics.h"
#include <chrono>
#include <string>
#include <DirectXMath.h>
#include <Windows.h>
#include "CameraControl.h"
#include "Saving.h"

App::App()
    : wnd(1920, 1080, "Untitled GameEngine", true)
    , cam(wnd.Gfx().GetCamera())
{
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
}

    
int App::Go()
{
    Saving saving;
    saving.SaveAll(Drawables);
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

void App::AddAMesh(const std::string& Path, const std::string& Name, DirectX::XMFLOAT3 pos)
{
    Drawables.emplace_back();
    auto& obj = Drawables.back();

    obj.mesh.Load(Path, wnd.Gfx().GetDevice());
    obj.Name = Name;
    obj.Anchored = true;
    obj.pos = pos;
    obj.Velocity = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
    obj.Orientation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
    obj.color = DirectX::XMINT3(
        static_cast<int>(Color3.x * 255.0f),
        static_cast<int>(Color3.y * 255.0f),
        static_cast<int>(Color3.z * 255.0f));
}
void App::DoFrame(float deltaTime)
{
    wnd.Gfx().ClearBuffer(0.0f, 0.0f, 0.1f);

    CameraControl camC;
    camC.MakeCameraControls(wnd, deltaTime);

    for (auto& obj : Drawables)
    {
        wnd.Gfx().DrawMesh(deltaTime, obj.mesh, obj.Orientation, obj.pos, obj.color, obj.Velocity, obj.Anchored);
    }

    wnd.mouse.Update();

    SetWindowTextW(wnd.hWnd, std::to_wstring(1.0f / deltaTime).c_str());

    if (wnd.kbd.KeyIsPressed(VK_CONTROL) && wnd.kbd.KeyIsPressed(0x35))
    {
        AddAMesh(assets+"\\UntitledSUS.fbx", "Sus", DirectX::XMFLOAT3(0.0f, 5.0f, 0.0f));
    }

    makeGui.MakeIMGui(
        Drawables,
        [this](const std::string& path, const std::string& name, DirectX::XMFLOAT3 pos) {
            AddAMesh(path, name, pos);
        },
        reinterpret_cast<float*>(&Color3)
    );
    wnd.Gfx().EndFrame();

    if (wnd.kbd.KeyIsPressed(VK_TAB)) {
        std::exit(-25);
    }
}
