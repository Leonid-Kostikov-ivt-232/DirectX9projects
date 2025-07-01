#include "InitDX93D.h"


// Инициализация Direct3D
HRESULT InitD3D(HWND hWnd) {
    g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
    if (!g_pD3D) return E_FAIL;

    D3DPRESENT_PARAMETERS d3dpp = {};
    d3dpp.Windowed = FALSE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
    d3dpp.BackBufferWidth = SCREEN_WIDTH;
    d3dpp.BackBufferHeight = SCREEN_HEIGHT;
    d3dpp.hDeviceWindow = hWnd;

    HRESULT hr = g_pD3D->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        hWnd,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING,
        &d3dpp,
        &g_pd3dDevice);

    if (SUCCEEDED(hr)) {
        hr = D3DXCreateSprite(g_pd3dDevice, &g_pSprite);
    }
    return hr;
}
