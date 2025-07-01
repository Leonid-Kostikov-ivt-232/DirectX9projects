#include "dxfunc.h"

HRESULT DX3DInit(
    IDirect3D9 **ppiD3D9,
    IDirect3DDevice9 **ppiD3DDevice9,
    HWND hWnd,
    DWORD iWidth,
    DWORD iHeight,
    BOOL bFullScreen
) {
    if (!ppiD3D9 || !ppiD3DDevice9) return E_POINTER;

    *ppiD3D9 = Direct3DCreate9(D3D_SDK_VERSION);
    if (!*ppiD3D9) return E_FAIL;

    D3DPRESENT_PARAMETERS d3dpp = {};
    d3dpp.Windowed = !bFullScreen;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow = hWnd;
    d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
    d3dpp.BackBufferWidth = iWidth;
    d3dpp.BackBufferHeight = iHeight;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;

    HRESULT hr = (*ppiD3D9)->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        hWnd,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING,
        &d3dpp,
        ppiD3DDevice9
    );

    if (FAILED(hr)) {
        (*ppiD3D9)->Release();
        *ppiD3D9 = NULL;
        return hr;
    }

    return S_OK;
}
