#pragma once
#include "Window.h"
#include "Timer.h"
#include "Mesh.h"
#include "CameraControl.h"
#include "ImGuiManager.h"

class App
{
public:
    App();
    int Go();

    void AddAMesh(std::string Path, std::string Name, bool Anchored, XMFLOAT3 pos, XMFLOAT3 vel, XMFLOAT3 Orientation, XMINT3 color);

private:
    void DoFrame(float deltaTime); // Lisätty deltaTime-parametri

private:
    ImGuiManager imgui;
    CameraControl camC;
    Window wnd;
    Timer timer;
};
