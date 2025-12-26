#include "Graphics.h"
#include "Releaser.h"
#include "Window.h"
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <stdexcept>

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dcompiler.lib")

using namespace DirectX;

struct Vertex
{
    float x, y, z;
};

struct ConstantBuffer
{
    XMMATRIX transfrom;
};

struct PixelConstantBuffer
{
    XMFLOAT4 color;
};

Graphics::Graphics(HWND hWnd)
{
    HRESULT hr;

    DXGI_SWAP_CHAIN_DESC scd = {};
    scd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    scd.SampleDesc.Count = 1;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.BufferCount = 1;
    scd.OutputWindow = hWnd;
    scd.Windowed = TRUE;
    scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &scd,
        &pSwap,
        &pDevice,
        nullptr,
        &pContext
    );
    if (FAILED(hr))
        throw Window::Exception(__LINE__, __FILE__, hr);

    // ===== Back buffer =====
    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = pSwap->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
    if (FAILED(hr))
        throw Window::Exception(__LINE__, __FILE__, hr);

    hr = pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pTarget);
    pBackBuffer->Release();
    if (FAILED(hr))
        throw Window::Exception(__LINE__, __FILE__, hr);

    // 🔥 TÄMÄ PUUTTUI – ILMAN TÄTÄ EI PIIRRY MITÄÄN
    pContext->OMSetRenderTargets(1u, &pTarget, nullptr);

    // 🔥 TÄMÄ PUUTTUI – ILMAN TÄTÄ CLIPPI MENEE VÄÄRIN
    D3D11_VIEWPORT vp = {};
    vp.Width = 800.0f;
    vp.Height = 600.0f;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    pContext->RSSetViewports(1, &vp);

    D3D11_BUFFER_DESC cb = {};
    cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cb.ByteWidth = sizeof(ConstantBuffer);
    cb.Usage = D3D11_USAGE_DYNAMIC;
    cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    hr = pDevice->CreateBuffer(&cb, nullptr, &pConstantBuffer);
    if (FAILED(hr))
        throw Window::Exception(__LINE__, __FILE__, hr);

    D3D11_BUFFER_DESC pcb = {};
    pcb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    pcb.ByteWidth = sizeof(PixelConstantBuffer);
    pcb.Usage = D3D11_USAGE_DYNAMIC;
    pcb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    hr = pDevice->CreateBuffer(&pcb, nullptr, &pColorBuffer);
    if (FAILED(hr))
        throw Window::Exception(__LINE__, __FILE__, hr);

    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* psBlob = nullptr;

    hr = D3DCompileFromFile(L"VertexShader.hlsl", nullptr, nullptr, "main", "vs_5_0", 0, 0, &vsBlob, nullptr);
    if (FAILED(hr))
        throw Window::Exception(__LINE__, __FILE__, hr);

    hr = D3DCompileFromFile(L"PixelShader.hlsl", nullptr, nullptr, "main", "ps_5_0", 0, 0, &psBlob, nullptr);
    if (FAILED(hr))
        throw Window::Exception(__LINE__, __FILE__, hr);

    hr = pDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &pVS);
    if (FAILED(hr))
        throw Window::Exception(__LINE__, __FILE__, hr);

    hr = pDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pPS);
    if (FAILED(hr))
        throw Window::Exception(__LINE__, __FILE__, hr);

    D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        { "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 }
    };

    hr = pDevice->CreateInputLayout(ied, 1, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &pLayout);
    if (FAILED(hr))
        throw Window::Exception(__LINE__, __FILE__, hr);

    vsBlob->Release();
    psBlob->Release();
}

Graphics::~Graphics()
{
    Release(pVertexBuffer);
    Release(pIndexBuffer);
    Release(pConstantBuffer);
    Release(pColorBuffer);
    Release(pLayout);
    Release(pVS);
    Release(pPS);
    Release(pTarget);
    Release(pDepthStencilView);
    Release(pSwap);

    if (pContext)
    {
        pContext->ClearState();
        pContext->Flush();
    }

    Release(pContext);
    Release(pDevice);
}

void Graphics::EndFrame()
{
    pSwap->Present(1, 0);
}

void Graphics::DrawTestTriangle(float angle, int r, int g, int b)
{
    HRESULT hr;

    // ===== Render target =====
    pContext->OMSetRenderTargets(1u, &pTarget, nullptr);

    // ===== Viewport =====
    D3D11_VIEWPORT vp = {};
    vp.Width = 800.0f;
    vp.Height = 600.0f;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    pContext->RSSetViewports(1u, &vp);

    // ===== Vertex data =====
    struct Vertex { float x, y; };
    Vertex vertices[] =
    {
        {  0.0f,  0.5f },
        {  0.5f, -0.5f },
        { -0.5f, -0.5f }
    };

    D3D11_MAPPED_SUBRESOURCE msr;
    pContext->Map(pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
    memcpy(msr.pData, &cb, sizeof(cb));
    pContext->Unmap(pConstantBuffer, 0);

    PixelConstantBuffer pcb;
    pcb.color = XMFLOAT4(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);

    pContext->Map(pColorBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
    memcpy(msr.pData, &pcb, sizeof(pcb));
    pContext->Unmap(pColorBuffer, 0);

    UINT stride = sizeof(Vertex);
    UINT offset = 0u;
    pContext->IASetVertexBuffers(0u, 1u, &vb, &stride, &offset);

    // ===== Shaders =====
    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* psBlob = nullptr;
    ID3D11VertexShader* vs = nullptr;
    ID3D11PixelShader* ps = nullptr;

    if (FAILED(hr = D3DCompileFromFile(L"VertexShader.hlsl", nullptr, nullptr, "main", "vs_5_0", 0, 0, &vsBlob, nullptr)))
        throw Window::Exception::Exception(__LINE__, __FILE__, hr);

    if (FAILED(hr = D3DCompileFromFile(L"PixelShader.hlsl", nullptr, nullptr, "main", "ps_5_0", 0, 0, &psBlob, nullptr)))
        throw Window::Exception::Exception(__LINE__, __FILE__, hr);

    if (FAILED(hr = pDevice->CreateVertexShader(vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(),
        nullptr, &vs)))
        throw Window::Exception::Exception(__LINE__, __FILE__, hr);

    if (FAILED(hr = pDevice->CreatePixelShader(psBlob->GetBufferPointer(),
        psBlob->GetBufferSize(),
        nullptr, &ps)))
        throw Window::Exception::Exception(__LINE__, __FILE__, hr);

    // ===== Input layout =====
    ID3D11InputLayout* layout = nullptr;
    D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0,
          D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    if (FAILED(hr = pDevice->CreateInputLayout(ied, 1,
        vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(),
        &layout)))
        throw Window::Exception::Exception(__LINE__, __FILE__, hr);

    pContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
    pContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    pContext->IASetInputLayout(pLayout);
    pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    pContext->VSSetShader(pVS, nullptr, 0);
    pContext->VSSetConstantBuffers(0, 1, &pConstantBuffer);
    pContext->PSSetConstantBuffers(0, 1, &pColorBuffer);
    pContext->PSSetShader(pPS, nullptr, 0);

    pContext->DrawIndexed(AmountOfVerticies, 0, 0);
}
