#pragma once
#include <vector>
#include "Window.h"
#include "Object.h"
#include "MakeGui.h"
#include <DirectXMath.h>
#include "GLOBALS.h"

class App
{
public:
    App();
    int Go();

    void AddAMesh(const std::string& Path, const std::string& Name, DirectX::XMFLOAT3 pos);
    void DoFrame(float deltaTime);
    std::vector<Object> GetDrawables();
private:
    Window wnd;
    Camera cam;

    MakeGui makeGui;

    std::vector<Object> Drawables;

    DirectX::XMFLOAT4 Color3 = { 0,0,0,1 };
};
