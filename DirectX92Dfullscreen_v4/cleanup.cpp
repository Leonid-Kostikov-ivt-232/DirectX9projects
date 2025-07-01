#include "InitDX93D.h"
#include "cleanup.h"


// Очистка
void Cleanup() {
    if (g_pTexture) g_pTexture->Release();
    if (g_pSprite) g_pSprite->Release();
    if (g_pd3dDevice) g_pd3dDevice->Release();
    if (g_pD3D) g_pD3D->Release();
}
