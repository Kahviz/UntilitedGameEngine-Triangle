#include "Graphics.h"
#include "Releaser.h"
#include "Window.h"

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dcompiler.lib")

Graphics::Graphics(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    sd.SampleDesc.Count = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = 1;
    sd.OutputWindow = hWnd;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &sd,
        &pSwap,
        &pDevice,
        nullptr,
        &pContext
    );

    ID3D11Texture2D* pBackBuffer = nullptr;
    pSwap->GetBuffer(
        0,
        __uuidof(ID3D11Texture2D),
        (void**)&pBackBuffer
    );

    pDevice->CreateRenderTargetView(
        pBackBuffer,
        nullptr,
        &pTarget
    );
    pBackBuffer->Release();

    pContext->OMSetRenderTargets(1u, &pTarget, nullptr);

    D3D11_VIEWPORT vp = {};
    vp.Width = 800.0f;
    vp.Height = 600.0f;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    pContext->RSSetViewports(1u, &vp);
}


Graphics::~Graphics()
{
	Release(pTarget);
	Release(pContext);
	Release(pSwap);
	Release(pDevice);
}

void Graphics::EndFrame()
{
	pSwap->Present(1u, 0u);
}

void Graphics::DrawTestTriangle()
{
    HRESULT hr;

    pContext->OMSetRenderTargets(1u, &pTarget, nullptr);

    D3D11_VIEWPORT vp = {};
    vp.Width = 800.0f;
    vp.Height = 600.0f;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    pContext->RSSetViewports(1u, &vp);

    struct Vertex { float x, y; };
    Vertex vertices[] =
    {
        {  0.0f,  0.5f },
        {  0.5f, -0.5f },
        { -0.5f, -0.5f }
    };

    ID3D11Buffer* vb = nullptr;
    D3D11_BUFFER_DESC bd = {};
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.ByteWidth = sizeof(vertices);
    bd.Usage = D3D11_USAGE_DEFAULT;

    D3D11_SUBRESOURCE_DATA sd = {};
    sd.pSysMem = vertices;

    if (FAILED(hr = pDevice->CreateBuffer(&bd, &sd, &vb)))
        throw Window::Exception::Exception(__LINE__, __FILE__, hr);

    UINT stride = sizeof(Vertex);
    UINT offset = 0u;
    pContext->IASetVertexBuffers(0u, 1u, &vb, &stride, &offset);

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

    // ===== Pipeline setup =====
    pContext->IASetInputLayout(layout);
    pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    pContext->VSSetShader(vs, nullptr, 0u);
    pContext->PSSetShader(ps, nullptr, 0u);

    // ===== Draw =====
    pContext->Draw(3u, 0u);

    // ===== Cleanup =====
    Release(vb);
    Release(vs);
    Release(ps);
    Release(vsBlob);
    Release(psBlob);
    Release(layout);
}
