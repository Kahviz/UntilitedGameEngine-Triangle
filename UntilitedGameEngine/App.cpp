#include "App.h"
#include <thread>
#include <chrono>
#include "Graphics.h"

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
    static auto rr = 1;
    rr += 10;
    if (rr >= 255)
        rr = 0;

    float r = rr / 255.0f;
    float g = 245 / 255.0f;
    float b = 47 / 255.0f;

    wnd.Gfx().ClearBuffer(r, 0, 0);
    wnd.Gfx().DrawTestTriangle(timer.Peek(), rr, 245, 47); 
    wnd.Gfx().EndFrame();
}