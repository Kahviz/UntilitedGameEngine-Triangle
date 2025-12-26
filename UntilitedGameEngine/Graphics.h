#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "Mesh.h"
#include <string>

using namespace DirectX;

struct ConstantBuffer
{
    XMMATRIX transfrom;
};

struct PixelConstantBuffer
{
    XMFLOAT4 color;
};

class Graphics
{
public:
    Graphics(HWND hWnd);
    ~Graphics();
    Graphics(const Graphics&) = delete;
    Graphics& operator=(const Graphics&) = delete;

    void EndFrame();
    void LoadMesh(const std::string& filename);
    void DrawMesh(
        float deltaTime,
        Mesh& mesh,
        XMFLOAT3 Orientation,
        XMFLOAT3& pos,
        XMINT3 color,
        XMFLOAT3& Velocity,
        bool Anchored
    );

    void ClearBuffer(float r, float g, float b);

    ID3D11Device* pDevice = nullptr;

private:
    Mesh MeshManager;
    const float Gravity = 9.81f;
    int Fov = DirectX::XMConvertToRadians(90.0f);
    int AmountOfVerticies = 0;

    ID3D11DepthStencilView* pDepthStencilView = nullptr;
    ID3D11DeviceContext* pContext = nullptr;
    IDXGISwapChain* pSwap = nullptr;
    ID3D11RenderTargetView* pTarget = nullptr;
    ID3D11Buffer* pVertexBuffer = nullptr;
    ID3D11Buffer* pIndexBuffer = nullptr;
    ID3D11Buffer* pConstantBuffer = nullptr;
    ID3D11Buffer* pColorBuffer = nullptr;
    ID3D11VertexShader* pVS = nullptr;
    ID3D11PixelShader* pPS = nullptr;
    ID3D11InputLayout* pLayout = nullptr;

    ID3D11Buffer* pMeshVertexBuffer = nullptr;
    ID3D11Buffer* pMeshIndexBuffer = nullptr;
    UINT MeshIndexCount = 0;
};
