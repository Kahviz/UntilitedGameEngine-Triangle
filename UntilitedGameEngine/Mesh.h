#pragma once
#include <d3d11.h>
#include <vector>
#include <string>
#include <stdexcept>
#include "Object.h"
#include "App.h"

struct Vertex
{
    float x, y, z;
};

class Mesh
{
public:
    void Load(const std::string& file, ID3D11Device* device, Object obj);
    void Draw(ID3D11DeviceContext* ctx) const;

    std::vector<Vertex> GetVertices() const { return verts; }

private:
    ID3D11Buffer* vb = nullptr;
    ID3D11Buffer* ib = nullptr;
    UINT indexCount = 0;
    std::vector<Vertex> verts;
};
