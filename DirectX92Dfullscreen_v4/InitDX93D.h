#ifndef _INITDX93D_H_
#define _INITDX93D_H_


#include <d3d9.h>
#include <d3dx9.h>


// Глобальные объекты
extern LPDIRECT3D9 g_pD3D;
extern LPDIRECT3DDEVICE9 g_pd3dDevice;
extern LPD3DXSPRITE g_pSprite;
extern LPDIRECT3DTEXTURE9 g_pTexture;

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

HRESULT InitD3D(HWND hWnd);


#endif // _INITDX93D_H_
