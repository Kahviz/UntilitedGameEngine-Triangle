#pragma once
#include <d3d11.h>
#include <vector>
#include <string>

struct Vertex
{
    float x, y, z;
};

class Mesh
{
public:
    void Load(const std::string& file, ID3D11Device* device);
    void Draw(ID3D11DeviceContext* ctx) const;
private:
    ID3D11Buffer* vb = nullptr;
    ID3D11Buffer* ib = nullptr;
    UINT indexCount = 0;
};
