/*
imgui example debloated cuz i am a really cool person
current imgui version as of making this: v1.92.5
not saying you have to use this im just uploading to preserve
*/

#include <d3d11.h>
#include <tchar.h>

#include "Menu/Menu.hpp"

static ID3D11Device* g_Device = nullptr;
static ID3D11DeviceContext* g_Context = nullptr;
static IDXGISwapChain* g_SwapChain = nullptr;
static ID3D11RenderTargetView* g_RTV = nullptr;

static void InitStyle()
{
    // two main imgui styles
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();
}

bool CreateDevice(HWND hWnd);
void CleanupDevice();
void CreateRTV();
void CleanupRTV();
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int main()
{
    WNDCLASSEX wc = { sizeof(wc), CS_CLASSDC, WndProc, 0, 0, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Window", nullptr };

    RegisterClassEx(&wc);
    HWND hwnd = CreateWindow(wc.lpszClassName, L"ImGui DX11", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 720, nullptr, nullptr, wc.hInstance, nullptr);

    if (!CreateDevice(hwnd))
    {
        CleanupDevice();
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    InitStyle();

    // you can load fonts here too (check imgui docs)

    ImGui_ImplWin32_Init(hwnd); // init
    ImGui_ImplDX11_Init(g_Device, g_Context);

    MSG msg{};
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }

        // frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // menu
        Menu::RenderMenu();

        // optionally show the imgui demo window
        if (Menu::m_bShowDemo)
            ImGui::ShowDemoWindow();

        ImGui::Render();
        float clear[4] = { 0.1f, 0.1f, 0.1f, 1.0f };

        g_Context->OMSetRenderTargets(1, &g_RTV, nullptr);
        g_Context->ClearRenderTargetView(g_RTV, clear);

        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        g_SwapChain->Present(1, 0); // vsync
        // g_SwapChain->Present(0, 0); // no vsync
    }

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    CleanupDevice();
    DestroyWindow(hwnd);
    UnregisterClass(wc.lpszClassName, wc.hInstance);
    return 0;
}

bool CreateDevice(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC scd{};
    scd.BufferCount = 1;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = hWnd;
    scd.SampleDesc.Count = 1;
    scd.Windowed = TRUE;
    scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    if (D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &scd, &g_SwapChain, &g_Device, nullptr, &g_Context) != S_OK) return false;

    CreateRTV();
    return true;
}

void CleanupDevice()
{
    CleanupRTV();
    if (g_SwapChain) { g_SwapChain->Release(); g_SwapChain = nullptr; }
    if (g_Context) { g_Context->Release(); g_Context = nullptr; }
    if (g_Device) { g_Device->Release(); g_Device = nullptr; }
}

void CreateRTV()
{
    ID3D11Texture2D* backBuffer = nullptr;

    if (FAILED(g_SwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer))) || !backBuffer) return;

    g_Device->CreateRenderTargetView(backBuffer, nullptr, &g_RTV);
    backBuffer->Release();
}

void CleanupRTV()
{
    if (g_RTV) { g_RTV->Release(); g_RTV = nullptr; }
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_SwapChain && wParam != SIZE_MINIMIZED)
        {
            CleanupRTV();
            g_SwapChain->ResizeBuffers(0, LOWORD(lParam), HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            CreateRTV();
        }
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}