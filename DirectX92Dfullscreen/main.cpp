#include <d3d9.h>
#include <d3dx9.h>
#include <windows.h>

LPDIRECT3D9             g_pD3D = nullptr;
LPDIRECT3DDEVICE9       g_pd3dDevice = nullptr;
LPD3DXSPRITE            g_pSprite = nullptr;
LPDIRECT3DTEXTURE9      g_pTexture = nullptr;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// Прототипы функций
HRESULT InitD3D(HWND hWnd);
HRESULT LoadTexture();
void Render();
void Cleanup();
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Главная функция
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    WNDCLASSEX wc = {
        sizeof(WNDCLASSEX),
        CS_CLASSDC,
        WndProc, // Исправлено: используем наш WndProc вместо DefWindowProc
        0, 0,
        hInstance,
        NULL,
        LoadCursor(NULL, IDC_ARROW),
        NULL,
        NULL,
        "DX9_2D_Fullscreen",
        NULL
    };

    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Failed to register window class", "Error", MB_OK);
        return 0;
    }

    // Создаём полноэкранное окно
    HWND hWnd = CreateWindowEx(
        0,
        "DX9_2D_Fullscreen",
        "Direct3D 9 - Fullscreen 2D",
        WS_POPUP,
        0, 0,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        NULL, NULL, hInstance, NULL);

    if (!hWnd)
    {
        MessageBox(NULL, "Window creation failed", "Error", MB_OK);
        return 0;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // Инициализация Direct3D
    if (FAILED(InitD3D(hWnd)))
    {
        MessageBox(hWnd, "Failed to initialize Direct3D", "Error", MB_OK);
        Cleanup();
        return 0;
    }

    // Загрузка текстуры
    if (FAILED(LoadTexture()))
    {
        MessageBox(hWnd, "Failed to load texture", "Error", MB_OK);
        Cleanup();
        return 0;
    }

    // Цикл сообщений с PeekMessage
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    while (TRUE)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                break;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            Render();
        }
    }

    Cleanup();
    UnregisterClass("DX9_2D_Fullscreen", wc.hInstance);
    return (int)msg.wParam;
}

// Обработчик сообщений окна
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_LBUTTONDOWN:
        PostQuitMessage(0);
        return 0;

    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE)
        {
            PostQuitMessage(0);
            return 0;
        }
        break;

    case WM_ACTIVATE:
        // Обработка активации/деактивации окна
        break;

    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}

// Остальные функции остаются без изменений
HRESULT InitD3D(HWND hWnd)
{
    g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
    if (!g_pD3D)
        return E_FAIL;

    D3DPRESENT_PARAMETERS d3dpp = {};
    d3dpp.Windowed = FALSE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
    d3dpp.BackBufferWidth = SCREEN_WIDTH;
    d3dpp.BackBufferHeight = SCREEN_HEIGHT;
    d3dpp.BackBufferCount = 1;
    d3dpp.hDeviceWindow = hWnd;
    d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

    HRESULT hr = g_pD3D->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        hWnd,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING,
        &d3dpp,
        &g_pd3dDevice);

    if (SUCCEEDED(hr))
    {
        hr = D3DXCreateSprite(g_pd3dDevice, &g_pSprite);
    }

    return hr;
}

HRESULT LoadTexture()
{
    return D3DXCreateTextureFromFile(g_pd3dDevice, "background.bmp", &g_pTexture);
}

void Render()
{
    if (!g_pd3dDevice)
        return;

    g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(50, 50, 50), 1.0f, 0);
    if (SUCCEEDED(g_pd3dDevice->BeginScene()))
    {
        g_pSprite->Begin(D3DXSPRITE_ALPHABLEND);

        D3DSURFACE_DESC desc;
        g_pTexture->GetLevelDesc(0, &desc);

        float scaleX = (float)SCREEN_WIDTH / desc.Width;
        float scaleY = (float)SCREEN_HEIGHT / desc.Height;

        D3DXMATRIX matScale;
        D3DXMatrixScaling(&matScale, scaleX, scaleY, 1.0f);
        g_pSprite->SetTransform(&matScale);

        D3DXVECTOR3 position(0, 0, 0);
        g_pSprite->Draw(g_pTexture, NULL, NULL, &position, D3DCOLOR_XRGB(255, 255, 255));

        g_pSprite->End();
        g_pd3dDevice->EndScene();
    }
    g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}

void Cleanup()
{
    if (g_pTexture) g_pTexture->Release();
    if (g_pSprite) g_pSprite->Release();
    if (g_pd3dDevice) g_pd3dDevice->Release();
    if (g_pD3D) g_pD3D->Release();
}
