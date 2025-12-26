#pragma once
#include "Window.h"
#include "Timer.h"
#include "Mesh.h"
#include "CameraControl.h"

class App
{
public:
    App();
    int Go();

private:
    void DoFrame(float deltaTime); // Lisätty deltaTime-parametri

private:
    CameraControl camC;
    Window wnd;
    Timer timer;
};
