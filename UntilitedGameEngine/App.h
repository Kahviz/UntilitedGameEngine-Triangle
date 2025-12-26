#pragma once
#include "Window.h"
#include "Timer.h"
#include "Mesh.h"

class App
{
public:
    App();
    int Go();

private:
    void DoFrame(float deltaTime); // Lisätty deltaTime-parametri

private:
    Window wnd;
    Timer timer;
};
