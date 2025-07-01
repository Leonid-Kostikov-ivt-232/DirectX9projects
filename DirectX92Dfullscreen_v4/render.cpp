#include "InitDX93D.h"
#include "render.h"


// Πενδεπ
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
