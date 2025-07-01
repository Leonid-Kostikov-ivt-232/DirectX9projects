#ifndef _DXFUNC_H_
#define _DXFUNC_H_

#include <d3d9.h>
#include <d3dx9.h>

HRESULT DX3DInit(IDirect3D9 **ppiD3D9,
                 IDirect3DDevice9 **ppiD3Device9,
                 HWND hWnd,
                 DWORD iWidth,
                 DWORD iHeight,
                 BOOL bFullScreen
                 );

#endif // _DXFUNC_H_
