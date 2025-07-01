#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>
#include <iostream>

//#pragma comment(lib, "dsound.lib")
//#pragma comment(lib, "dxguid.lib")
//#pragma comment(lib, "winmm.lib") // ��� ������� LoadSound

// ���������� ����������
LPDIRECTSOUND8 g_pDS = nullptr;
LPDIRECTSOUNDBUFFER g_pDSBuffer = nullptr;


// ������� ��� ������������� DirectSound
bool InitDirectSound(HWND hWnd)
{
    if (FAILED(DirectSoundCreate8(NULL, &g_pDS, NULL)))
        return false;

    if (FAILED(g_pDS->SetCooperativeLevel(hWnd, DSSCL_PRIORITY)))
        return false;

    // ������� ��������� ����� (�������������, �� �������������)
    DSBUFFERDESC dsbd = {};
    dsbd.dwSize = sizeof(DSBUFFERDESC);
    dsbd.dwFlags = DSBCAPS_PRIMARYBUFFER;

    LPDIRECTSOUNDBUFFER pPrimaryBuffer = nullptr;
    if (FAILED(g_pDS->CreateSoundBuffer(&dsbd, &pPrimaryBuffer, NULL)))
        return false;

    // ������������� ������ ���������� ������ (��������, 44.1kHz, 16 ���, ������)
    WAVEFORMATEX wfx = {};
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = 2;
    wfx.nSamplesPerSec = 44100;
    wfx.wBitsPerSample = 16;
    wfx.nBlockAlign = wfx.nChannels * wfx.wBitsPerSample / 8;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

    if (FAILED(pPrimaryBuffer->SetFormat(&wfx)))
    {
        pPrimaryBuffer->Release();
        return false;
    }

    pPrimaryBuffer->Release();
    return true;
}

// ������� ��� �������� WAV-����� � �����
bool LoadWaveFile(const char* filename)
{
    // ��������� WAV-���� � ������� Windows API
    HMMIO hmmio = mmioOpenA(const_cast<LPSTR>(filename), NULL, MMIO_READ | MMIO_ALLOCBUF);
    if (!hmmio)
        return false;

    // ������ ��������� RIFF
    MMCKINFO mmckinfoParent = {};
    mmckinfoParent.fccType = mmioFOURCC('W', 'A', 'V', 'E');
    if (mmioDescend(hmmio, &mmckinfoParent, NULL, MMIO_FINDRIFF) != MMSYSERR_NOERROR)
    {
        mmioClose(hmmio, 0);
        return false;
    }

    // ������ ������
    MMCKINFO mmckinfoSubchunk = {};
    mmckinfoSubchunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
    if (mmioDescend(hmmio, &mmckinfoSubchunk, &mmckinfoParent, 0) != MMSYSERR_NOERROR)
    {
        mmioClose(hmmio, 0);
        return false;
    }

    WAVEFORMATEX wfx = {};
    if (mmioRead(hmmio, (HPSTR)&wfx, sizeof(WAVEFORMATEX)) != sizeof(WAVEFORMATEX))
    {
        mmioClose(hmmio, 0);
        return false;
    }

    mmioAscend(hmmio, &mmckinfoSubchunk, 0);

    // ������ ������ �����
    mmckinfoSubchunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
    if (mmioDescend(hmmio, &mmckinfoSubchunk, &mmckinfoParent, MMIO_FINDCHUNK) != MMSYSERR_NOERROR)
    {
        mmioClose(hmmio, 0);
        return false;
    }

    // ������� ��������� ����� ��� �����
    DSBUFFERDESC dsbd = {};
    dsbd.dwSize = sizeof(DSBUFFERDESC);
    dsbd.dwFlags = DSBCAPS_CTRLVOLUME;
    dsbd.dwBufferBytes = mmckinfoSubchunk.cksize;
    dsbd.lpwfxFormat = &wfx;

    if (FAILED(g_pDS->CreateSoundBuffer(&dsbd, &g_pDSBuffer, NULL)))
    {
        mmioClose(hmmio, 0);
        return false;
    }

    // ��������� ����� � �������� ������
    void* pData1 = nullptr;
    void* pData2 = nullptr;
    DWORD dwLength1 = 0;
    DWORD dwLength2 = 0;

    if (FAILED(g_pDSBuffer->Lock(0, mmckinfoSubchunk.cksize, &pData1, &dwLength1, &pData2, &dwLength2, 0)))
    {
        g_pDSBuffer->Release();
        g_pDSBuffer = nullptr;
        mmioClose(hmmio, 0);
        return false;
    }

    mmioRead(hmmio, (HPSTR)pData1, dwLength1);
    if (pData2)
        mmioRead(hmmio, (HPSTR)pData2, dwLength2);

    g_pDSBuffer->Unlock(pData1, dwLength1, pData2, dwLength2);

    mmioClose(hmmio, 0);
    return true;
}

// ������� ��������������� �����
void PlaySoundBuffer()
{
    if (g_pDSBuffer)
    {
        g_pDSBuffer->SetCurrentPosition(0);
        g_pDSBuffer->Play(0, 0, DSBPLAY_LOOPING); // ����������� ������
    }
}

// ������� ��������
void Cleanup()
{
    if (g_pDSBuffer)
    {
        g_pDSBuffer->Release();
        g_pDSBuffer = nullptr;
    }
    if (g_pDS)
    {
        g_pDS->Release();
        g_pDS = nullptr;
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    // ������� ������� ����, ����������� ��� DirectSound
    WNDCLASS wc = {};
    wc.lpfnWndProc = DefWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "DSoundWindowClass";
    RegisterClass(&wc);

    HWND hWnd = CreateWindow("DSoundWindowClass", "DirectSound Test", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 300, 200, NULL, NULL, hInstance, NULL);

    if (!InitDirectSound(hWnd))
    {
        MessageBox(NULL, "�� ������� ���������������� DirectSound", "������", MB_OK);
        return 1;
    }

    if (!LoadWaveFile("TMNT_79.wav"))
    {
        MessageBox(NULL, "�� ������� ��������� WAV-����", "������", MB_OK);
        Cleanup();
        return 1;
    }

    PlaySoundBuffer();

    MessageBox(NULL, "������� OK ��� ������", "DirectSound", MB_OK);

    Cleanup();
    return 0;
}
