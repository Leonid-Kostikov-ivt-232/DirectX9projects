#include <windows.h>
#include <d3d9.h>

// Глобальные переменные
LPDIRECT3D9             g_pD3D = NULL;
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL;

RECT g_rect = { 100, 100, 200, 200 }; // Прямоугольник для drag and drop
bool g_dragging = false;
POINT g_dragOffset = {0, 0};

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_LBUTTONDOWN:
    {
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);

        // Проверяем, попал ли курсор в прямоугольник
        if (x >= g_rect.left && x <= g_rect.right && y >= g_rect.top && y <= g_rect.bottom)
        {
            g_dragging = true;
            // Запоминаем смещение курсора относительно левого верхнего угла прямоугольника
            g_dragOffset.x = x - g_rect.left;
            g_dragOffset.y = y - g_rect.top;
            SetCapture(hWnd); // Захватываем мышь
        }
        break;
    }
    case WM_MOUSEMOVE:
    {
        if (g_dragging)
        {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);

            // Обновляем позицию прямоугольника с учетом смещения
            int width = g_rect.right - g_rect.left;
            int height = g_rect.bottom - g_rect.top;

            g_rect.left = x - g_dragOffset.x;
            g_rect.top = y - g_dragOffset.y;
            g_rect.right = g_rect.left + width;
            g_rect.bottom = g_rect.top + height;

            InvalidateRect(hWnd, NULL, FALSE); // Перерисовать окно
        }
        break;
    }
    case WM_LBUTTONUP:
    {
        if (g_dragging)
        {
            g_dragging = false;
            ReleaseCapture();
        }
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void Render()
{
    if (g_pd3dDevice == NULL)
        return;

    g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(50, 50, 50), 1.0f, 0);
    g_pd3dDevice->BeginScene();

    // Рисуем прямоугольник (простой цветной квадратик)
    D3DRECT d3drect = { g_rect.left, g_rect.top, g_rect.right, g_rect.bottom };
    g_pd3dDevice->Clear(1, &d3drect, D3DCLEAR_TARGET, D3DCOLOR_XRGB(200, 0, 0), 1.0f, 0);

    g_pd3dDevice->EndScene();
    g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}

HRESULT InitD3D(HWND hWnd)
{
    g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
    if (!g_pD3D) return E_FAIL;

    D3DPRESENT_PARAMETERS d3dpp = {};
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow = hWnd;

    if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_pd3dDevice)))
    {
        return E_FAIL;
    }
    return S_OK;
}

void Cleanup()
{
    if (g_pd3dDevice) g_pd3dDevice->Release();
    if (g_pD3D) g_pD3D->Release();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0, 0,
                      GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                      "DX9DragDropClass", NULL };
    RegisterClassEx(&wc);

    HWND hWnd = CreateWindow(wc.lpszClassName, "DirectX9 Drag and Drop Example",
                             WS_OVERLAPPEDWINDOW, 100, 100, 800, 600,
                             NULL, NULL, wc.hInstance, NULL);

    if (FAILED(InitD3D(hWnd)))
    {
        Cleanup();
        return 0;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    ZeroMemory(&msg, sizeof(msg));

    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            Render();
        }
    }

    Cleanup();
    UnregisterClass(wc.lpszClassName, wc.hInstance);
    return 0;
}
