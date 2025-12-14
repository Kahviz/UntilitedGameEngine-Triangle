#include "App.h"
#include <thread>
#include <chrono>

auto const NAME = "Untitled GameEngine";

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
        // K‰sitell‰‰n ikkunan tapahtumat
        if (const auto ecode = Window::ProcessMessages())
        {
            return *ecode;
        }

        // Lasketaan delta-aika edellisest‰ framesta
        auto now = clock::now();
        std::chrono::duration<float> delta = now - last;
        last = now;

        DoFrame(delta.count());

        // Yritet‰‰n pit‰‰ ~60 FPS
        auto frameTime = clock::now() - now;
        auto sleepTime = std::chrono::milliseconds(16) - frameTime;
        if (sleepTime > std::chrono::milliseconds(0))
            std::this_thread::sleep_for(sleepTime);
    }
}

void App::DoFrame(float deltaTime)
{
    static auto r = 0.0f;
    r += 0.01f;
    if (r >= 1)
    {
        r = 0;
    }
    wnd.Gfx().ClearBuffer(r, r, 0);

    wnd.Gfx().DrawTestTriangle();

    wnd.Gfx().EndFrame();
}
