#pragma once
#include <string>
#include <DirectXMath.h>
#include "Mesh.h"

struct Object
{
    Mesh mesh;
    DirectX::XMFLOAT3 pos = { 0.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT3 Velocity = { 0.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT3 Orientation = { 0.0f, 0.0f, 0.0f };
    DirectX::XMINT3 color = { 255, 255, 255 };
    bool Anchored = true;
    std::string Name = "Obj";
    bool Selected = false;
};
