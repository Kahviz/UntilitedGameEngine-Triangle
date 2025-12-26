#include "App.h"
#include <thread>
#include <chrono>
#include "Graphics.h"
#include <vector>
#include "Mesh.h"
#include <DirectXMath.h>

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
};

std::vector<Object> Drawables;

App::App()
    : wnd(800, 600, NAME)
{
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

        auto frameTime = clock::now() - now;
        auto sleepTime = std::chrono::milliseconds(16) - frameTime;
        if (sleepTime > std::chrono::milliseconds(0))
            std::this_thread::sleep_for(sleepTime);
    }
}

void App::DoFrame(float deltaTime)
{
    wnd.Gfx().ClearBuffer(0, 0, 0);

    if (wnd.kbd.KeyIsPressed(VK_CONTROL))
    {
        if (wnd.kbd.KeyIsPressed(0x35))
        {
            Drawables.emplace_back();
            auto& obj = Drawables.back();

            obj.mesh.Load("Assets\\3DObjects\\UntitledSUS.fbx", wnd.Gfx().pDevice);
            obj.Anchored = false;
            obj.pos = { 0.0f, 5.0f, 0.0f };
            obj.Velocity = { 0.0f, 0.0f, 0.0f };
            obj.Orientation = { 0.0f, 0.0f, 0.0f };
            obj.color = { 255, 0, 0 };
        }
    }

    for (auto& obj : Drawables)
    {
        wnd.Gfx().DrawMesh(deltaTime, obj.mesh, obj.Orientation, obj.pos, obj.color, obj.Velocity, obj.Anchored);
    }

    wnd.Gfx().EndFrame();
}
