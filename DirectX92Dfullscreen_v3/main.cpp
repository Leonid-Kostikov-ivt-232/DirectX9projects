#include <d3d9.h>
#include <d3dx9.h>
#include <windows.h>
#include <fstream>
#include <unordered_map>
#include <string>
#include <vector>


// Глобальные объекты
LPDIRECT3D9 g_pD3D = nullptr;
LPDIRECT3DDEVICE9 g_pd3dDevice = nullptr;
LPD3DXSPRITE g_pSprite = nullptr;
LPDIRECT3DTEXTURE9 g_pTexture = nullptr;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// Структуры PAK
#pragma pack(push, 1)
struct PakHeader {
    uint32_t magic;
    uint32_t entries;
};

struct PakEntry {
    char name[32];
    uint32_t offset;
    uint32_t size;
};
#pragma pack(pop)

// Менеджер ресурсов
class PakLoader {
    std::ifstream pak;
    std::unordered_map<std::string, PakEntry> catalog;

public:
    bool Open(const char* path) {
        pak.open(path, std::ios::binary);
        if (!pak) return false;

        PakHeader header;
        pak.read(reinterpret_cast<char*>(&header), sizeof(header));

        if (header.magic != 0x4B4150) {
            pak.close();
            return false;
        }

        for (uint32_t i = 0; i < header.entries; ++i) {
            PakEntry entry;
            pak.read(reinterpret_cast<char*>(&entry), sizeof(entry));
            catalog[entry.name] = entry;
        }

        return true;
    }

    std::vector<char> Load(const char* name) {
        auto it = catalog.find(name);
        if (it == catalog.end()) return {};

        PakEntry& entry = it->second;
        std::vector<char> data(entry.size);

        pak.seekg(entry.offset);
        pak.read(data.data(), entry.size);

        return data;
    }
};

// Прототипы функций
HRESULT InitD3D(HWND hWnd);
LPDIRECT3DTEXTURE9 LoadTextureFromPak(PakLoader& loader, const char* name);
void Render();
void Cleanup();
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Точка входа
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    // Регистрация класса окна
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

    // Создание окна
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

    // Инициализация Direct3D
    if (FAILED(InitD3D(hWnd))) {
        MessageBox(hWnd, "Failed to initialize Direct3D", "Error", MB_OK);
        Cleanup();
        return 0;
    }

    // Загрузка PAK-архива
    PakLoader pak;
    if (!pak.Open("resources.pak")) {
        MessageBox(hWnd, "Failed to load PAK archive", "Error", MB_OK);
        Cleanup();
        return 0;
    }

    // Загрузка текстуры
    g_pTexture = LoadTextureFromPak(pak, "texture"); // Здесь вводим ключ словаря files_to_pack = {"texture": "background.png"}
    if (!g_pTexture) {
        MessageBox(hWnd, "Failed to load texture from PAK", "Error", MB_OK);
        Cleanup();
        return 0;
    }

    // Основной цикл
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

// Обработчик сообщений
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

// Загрузка текстуры из PAK
LPDIRECT3DTEXTURE9 LoadTextureFromPak(PakLoader& loader, const char* name) {
    auto data = loader.Load(name);
    if (data.empty()) return nullptr;

    LPDIRECT3DTEXTURE9 texture = nullptr;
    D3DXCreateTextureFromFileInMemory(
        g_pd3dDevice,
        data.data(),
        data.size(),
        &texture
    );

    return texture;
}

// Рендер
void Render() {
    if (!g_pd3dDevice || !g_pTexture) return;

    g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(50, 50, 50), 1.0f, 0);

    if (SUCCEEDED(g_pd3dDevice->BeginScene())) {
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

// Очистка
void Cleanup() {
    if (g_pTexture) g_pTexture->Release();
    if (g_pSprite) g_pSprite->Release();
    if (g_pd3dDevice) g_pd3dDevice->Release();
    if (g_pD3D) g_pD3D->Release();
}
