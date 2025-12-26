#pragma once
#include <d3d11.h>
#include "Window.h"
#include <DirectXMath.h>
#include <stdexcept>


class Graphics
{
public:
    Graphics(HWND hWnd);
    ~Graphics();
    Graphics(const Graphics&) = delete;
    Graphics& operator=(const Graphics&) = delete;

    void EndFrame();
    void DrawTestTriangle(float angle,int r,int g,int b);
    void ClearBuffer(float r, float g, float b)
    {

        const float color[] = { r,g,b, 1.0f };
        pContext->ClearRenderTargetView(pTarget, color);

        pContext->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
    }
private:
    int Fov = DirectX::XMConvertToRadians(90);
    int AmountOfVerticies = 0;
    ID3D11DepthStencilView* pDepthStencilView = nullptr;
    ID3D11Device* pDevice = nullptr;
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
};