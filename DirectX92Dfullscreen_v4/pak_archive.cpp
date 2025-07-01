#include "InitDX93D.h"
#include "pak_archive.h"


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
