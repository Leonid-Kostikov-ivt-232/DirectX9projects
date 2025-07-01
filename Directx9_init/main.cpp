#include <windows.h>
#include "dxfunc.h"

// Глобальные переменные
IDirect3D9       *pD3D       = NULL;
IDirect3DDevice9 *pD3DDevice = NULL;

// Прототипы функций
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL InitInstance(HINSTANCE, int);

// Точка входа
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    if (!InitInstance(hInstance, nCmdShow))
        return 0;

    // Цикл сообщений
    MSG msg = {};
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            // Здесь можно выполнять рендеринг
            if (pD3DDevice)
            {
                pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,40,100), 1.0f, 0);
                pD3DDevice->BeginScene();
                // ... Рисование ...
                pD3DDevice->EndScene();
                pD3DDevice->Present(NULL, NULL, NULL, NULL);
            }
        }
    }

    // Очистка ресурсов
    if (pD3DDevice) pD3DDevice->Release();
    if (pD3D) pD3D->Release();

    return (int)msg.wParam;
}

// Создание окна и инициализация Direct3D
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    // Регистрация класса окна
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "DX9DemoClass";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);

    if (!RegisterClass(&wc))
        return FALSE;

    // Создание окна
    HWND hWnd = CreateWindow(
        "DX9DemoClass", "DirectX 9 Demo",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, NULL, hInstance, NULL
    );

    if (!hWnd)
        return FALSE;

    // Вызов функции инициализации Direct3D
    if (DX3DInit(&pD3D, &pD3DDevice, hWnd, 800, 600, FALSE) != S_OK)
    {
        MessageBox(hWnd, "Ошибка инициализации DirectX", "Error", MB_OK);
        return FALSE;
    }

    // Отображение и обновление окна
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

// Оконная процедура
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
