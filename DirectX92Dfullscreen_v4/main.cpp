#include <windows.h>
#include "InitDX93D.h"
#include "pak_archive.h"
#include "render.h"
#include "cleanup.h"


// ���������� �������
LPDIRECT3D9 g_pD3D = nullptr;
LPDIRECT3DDEVICE9 g_pd3dDevice = nullptr;
LPD3DXSPRITE g_pSprite = nullptr;
LPDIRECT3DTEXTURE9 g_pTexture = nullptr;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;


// ��������� �������
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// ����� �����
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    // ����������� ������ ����
    WNDCLASSEX wc = {
        sizeof(WNDCLASSEX),
        CS_CLASSDC,
        WndProc,
        0, 0,
        hInstance,
        NULL,
        LoadCursor(NULL, IDC_ARROW),
        NULL,
        NULL,
        "DX9_PAK_Demo",
        NULL
    };

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, "Failed to register window class", "Error", MB_OK);
        return 0;
    }

    // �������� ����
    HWND hWnd = CreateWindowEx(
        0,
        "DX9_PAK_Demo",
        "Direct3D 9 - PAK Resources",
        WS_POPUP,
        0, 0,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        NULL, NULL, hInstance, NULL);

    if (!hWnd) {
        MessageBox(NULL, "Window creation failed", "Error", MB_OK);
        return 0;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // ������������� Direct3D
    if (FAILED(InitD3D(hWnd))) {
        MessageBox(hWnd, "Failed to initialize Direct3D", "Error", MB_OK);
        Cleanup();
        return 0;
    }

    // �������� PAK-������
    PakLoader pak;
    if (!pak.Open("resources.pak")) {
        MessageBox(hWnd, "Failed to load PAK archive", "Error", MB_OK);
        Cleanup();
        return 0;
    }

    // �������� ��������
    g_pTexture = LoadTextureFromPak(pak, "texture"); // ����� ������ ���� ������� files_to_pack = {"texture": "background.png"}
    if (!g_pTexture) {
        MessageBox(hWnd, "Failed to load texture from PAK", "Error", MB_OK);
        Cleanup();
        return 0;
    }

    // �������� ����
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    while (TRUE) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
            Render();
        }
    }

    Cleanup();
    UnregisterClass("DX9_PAK_Demo", wc.hInstance);
    return (int)msg.wParam;
}

// ���������� ���������
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_DESTROY:
    case WM_LBUTTONDOWN:
        PostQuitMessage(0);
        return 0;
    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE) PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}
