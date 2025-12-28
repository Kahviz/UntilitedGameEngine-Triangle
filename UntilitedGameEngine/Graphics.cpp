#include "Graphics.h"
#include "Window.h"
#include "Releaser.h"
#include <d3dcompiler.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stdexcept>
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dcompiler.lib")

using namespace DirectX;

Graphics::Graphics(HWND hWnd)
{
    //Kusinen Camera
    camera.SetProjectionValues(
        100.0f,
        static_cast<float>(screen_width) / static_cast<float>(screen_height),
        0.5f,
        1000.0f
    );

    camera.SetPosition(0.0f, 0.0f, -5.0f);

    // Swap chain
    DXGI_SWAP_CHAIN_DESC scd = {};
    scd.BufferDesc.Width = screen_width;
    scd.BufferDesc.Height = screen_height;
    scd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    scd.BufferDesc.RefreshRate.Numerator = 60;
    scd.BufferDesc.RefreshRate.Denominator = 1;
    scd.SampleDesc.Count = 1;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.BufferCount = 1;
    scd.OutputWindow = hWnd;
    scd.Windowed = TRUE;
    scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        nullptr, 0,
        D3D11_SDK_VERSION,
        &scd,
        &pSwap,
        &pDevice,
        nullptr,
        &pContext
    );

    if (FAILED(hr)) throw std::runtime_error("Failed to create D3D11 device and context");

    // Back buffer
    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = pSwap->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
    if (FAILED(hr)) throw std::runtime_error("Failed to get back buffer");

    hr = pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pTarget);
    pBackBuffer->Release();
    if (FAILED(hr)) throw std::runtime_error("Failed to create render target view");

    // Depth stencil
    D3D11_TEXTURE2D_DESC depthDesc = {};
    depthDesc.Width = screen_width;
    depthDesc.Height = screen_height;
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.Usage = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    ID3D11Texture2D* pDepthStencil = nullptr;
    hr = pDevice->CreateTexture2D(&depthDesc, nullptr, &pDepthStencil);
    if (FAILED(hr)) throw std::runtime_error("Failed to create depth stencil texture");

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = depthDesc.Format;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;

    hr = pDevice->CreateDepthStencilView(pDepthStencil, &dsvDesc, &pDepthStencilView);
    pDepthStencil->Release();
    if (FAILED(hr)) throw std::runtime_error("Failed to create depth stencil view");

    pContext->OMSetRenderTargets(1, &pTarget, pDepthStencilView);

    // Viewport
    D3D11_VIEWPORT vp = {};
    vp.Width = screen_width;
    vp.Height = screen_height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    pContext->RSSetViewports(1, &vp);

    // Constant buffers
    D3D11_BUFFER_DESC cbd = {};
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.ByteWidth = sizeof(ConstantBuffer);
    cbd.Usage = D3D11_USAGE_DYNAMIC;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    hr = pDevice->CreateBuffer(&cbd, nullptr, &pConstantBuffer);
    if (FAILED(hr)) throw std::runtime_error("Failed to create constant buffer");

    D3D11_BUFFER_DESC pcbd = {};
    pcbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    pcbd.ByteWidth = sizeof(PixelConstantBuffer);
    pcbd.Usage = D3D11_USAGE_DYNAMIC;
    pcbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    hr = pDevice->CreateBuffer(&pcbd, nullptr, &pColorBuffer);
    if (FAILED(hr)) throw std::runtime_error("Failed to create pixel constant buffer");

    // Compile shaders
    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* psBlob = nullptr;

    hr = D3DReadFileToBlob(L"VertexShader.cso", &vsBlob);
    if (FAILED(hr)) throw std::runtime_error("Failed to load vertex shader CSO");

    hr = D3DReadFileToBlob(L"PixelShader.cso", &psBlob);
    if (FAILED(hr))
    {
        hr = D3DCompileFromFile(L"PixelShader.hlsl", nullptr, nullptr, "main", "ps_5_0", 0, 0, &psBlob, nullptr);
        if (FAILED(hr))
        {
            throw std::runtime_error("Failed to load or compile pixel shader");
        }
    }

    hr = pDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &pVS);
    if (FAILED(hr)) throw std::runtime_error("Failed to create vertex shader Create");

    hr = pDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pPS);
    if (FAILED(hr)) throw std::runtime_error("Failed to create pixel shader Create");

    // Input layout
    D3D11_INPUT_ELEMENT_DESC ied[] = { {"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0} };
    hr = pDevice->CreateInputLayout(ied, 1, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &pLayout);
    if (FAILED(hr)) throw std::runtime_error("Failed to create input layout");

    Release(vsBlob);
    Release(psBlob);
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
    Release(pGUIPS);
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

Camera& Graphics::GetCamera()
{
    return camera;
}

ID3D11Device* Graphics::GetDevice() noexcept
{
    return pDevice;
}

ID3D11DeviceContext* Graphics::GetpContext() noexcept
{
    return pContext;
}

void Graphics::EndFrame()
{
    pSwap->Present(1, 0);
}

void Graphics::DrawMesh(float deltaTime, Mesh& mesh, XMFLOAT3 Orientation, XMFLOAT3& pos, XMINT3 color, XMFLOAT3& Velocity, bool Anchored)
{
    if (!pContext || !pConstantBuffer || !pColorBuffer)
        throw std::runtime_error("Graphics not properly initialized");

    XMMATRIX world = XMMatrixRotationRollPitchYaw(Orientation.x, Orientation.y, Orientation.z) *
        XMMatrixTranslation(pos.x, pos.y, pos.z);

    if (!Anchored)
    {
        Velocity.y -= Gravity * deltaTime;
        pos.y += Velocity.y * deltaTime;
    }

    // Yhdistä maailman, kameran ja projektion matriisit
    XMMATRIX view = camera.GetViewMatrix();
    XMMATRIX proj = camera.GetProjectionMatrix(); // Varmista, että SetProjectionValues on kutsuttu alustuksessa
    XMMATRIX transform = XMMatrixTranspose(world * view * proj);

    // Lähetä constant buffer GPU:lle
    ConstantBuffer cb;
    cb.transfrom = transform;

    D3D11_MAPPED_SUBRESOURCE msr;
    HRESULT hr = pContext->Map(pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
    if (FAILED(hr)) throw std::runtime_error("Failed to map constant buffer");
    memcpy(msr.pData, &cb, sizeof(cb));
    pContext->Unmap(pConstantBuffer, 0);

    // Lähetä väri pixel shaderille
    PixelConstantBuffer pcb = { XMFLOAT4(color.x / 255.f, color.y / 255.f, color.z / 255.f, 1.0f) };
    hr = pContext->Map(pColorBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
    if (FAILED(hr)) throw std::runtime_error("Failed to map pixel constant buffer");
    memcpy(msr.pData, &pcb, sizeof(pcb));
    pContext->Unmap(pColorBuffer, 0);

    // Aseta pipeline ja piirrä mesh
    pContext->IASetInputLayout(pLayout);
    pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    pContext->VSSetShader(pVS, nullptr, 0);
    pContext->PSSetShader(pPS, nullptr, 0);
    pContext->VSSetConstantBuffers(0, 1, &pConstantBuffer);
    pContext->PSSetConstantBuffers(0, 1, &pColorBuffer);

    mesh.Draw(pContext);
}

void Graphics::DrawTestTriangle(const XMFLOAT4& color)
{
    if (!pDevice || !pContext)
        throw std::runtime_error("Device or Context is null");
    if (!pVS || !pPS || !pLayout)
        throw std::runtime_error("Shaders or InputLayout not set");
    if (!pConstantBuffer)
        throw std::runtime_error("Constant buffer not set");

    struct Vertex
    {
        XMFLOAT3 position;
        XMFLOAT4 color;
    };

    Vertex vertices[] =
    {
        { XMFLOAT3(0.0f, 0.5f, 0.0f), color },
        { XMFLOAT3(0.5f,-0.5f, 0.0f), color },
        { XMFLOAT3(-0.5f,-0.5f, 0.0f), color }
    };


    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(vertices);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = vertices;

    ID3D11Buffer* pVertexBuffer = nullptr;
    HRESULT hr = pDevice->CreateBuffer(&bd, &initData, &pVertexBuffer);
    if (FAILED(hr)) throw std::runtime_error("Failed to create vertex buffer");

    XMMATRIX world = XMMatrixIdentity();
    XMMATRIX view = XMMatrixLookAtLH(XMVectorSet(0, 0, -0.01, 0), XMVectorSet(0, 0, 0, 0), XMVectorSet(0, 1, 0, 0));
    XMMATRIX proj = XMMatrixPerspectiveLH(1.0f, 1.0f, 0.009f, 100.0f);
    XMMATRIX transform = XMMatrixTranspose(world * view * proj);

    D3D11_MAPPED_SUBRESOURCE msr;
    hr = pContext->Map(pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
    if (FAILED(hr)) throw std::runtime_error("Failed to map constant buffer");

    struct ConstantBuffer
    {
        XMMATRIX transform;
    } cb;

    cb.transform = transform;
    memcpy(msr.pData, &cb, sizeof(cb));
    pContext->Unmap(pConstantBuffer, 0);

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    pContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
    pContext->IASetInputLayout(pLayout);
    pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    pContext->VSSetShader(pVS, nullptr, 0);
    pContext->PSSetShader(pPS, nullptr, 0);  // käytetään aina samaa pixel shaderia
    pContext->VSSetConstantBuffers(0, 1, &pConstantBuffer);

    PixelConstantBuffer pcb = { color }; // suoraan XMFLOAT4
    hr = pContext->Map(pColorBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
    if (FAILED(hr)) throw std::runtime_error("Failed to map pixel constant buffer");
    memcpy(msr.pData, &pcb, sizeof(pcb));
    pContext->Unmap(pColorBuffer, 0);

    pContext->PSSetConstantBuffers(0, 1, &pColorBuffer);

    pContext->Draw(3, 0);

    if (pVertexBuffer) pVertexBuffer->Release();
}


void Graphics::ClearBuffer(float r, float g, float b)
{
    if (!pContext) throw std::runtime_error("pContext is null");
    if (!pTarget) throw std::runtime_error("pTarget is null");
    if (!pDepthStencilView) throw std::runtime_error("pDepthStencilView is null");

    const float color[] = { r, g, b, 1.0f };
    pContext->ClearRenderTargetView(pTarget, color);
    pContext->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

