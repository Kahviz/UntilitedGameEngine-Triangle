#pragma once
#include <vector>
#include <functional>
#include "Object.h"
#include <DirectXMath.h>
#include "GLOBALS.h"

class MakeGui
{
public:
    void MakeIMGui(
        const std::vector<Object>& Drawables,
        std::function<void(const std::string&, const std::string&, DirectX::XMFLOAT3)> AddAMesh,
        float* Color3);
};