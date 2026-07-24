// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>
#include <iostream>
#include "game.h"
#include "hooks.h"
#include "vr.h"
#include "sdk.h"

#define VER_PRODUCT "Portal 2 VR Mod"
#define VER_VERSION "5.3.0"
#define VER_DATE    "2026-07-24"

static bool g_WindowPatched = false;

static void ForceWindowedMode()
{
    HWND hWnd = FindWindowA("Valve001", NULL);
    if (!hWnd)
        hWnd = FindWindowA(NULL, "Portal 2");
    if (!hWnd)
        return;

    LONG style = GetWindowLongPtrA(hWnd, GWL_STYLE);
    style &= ~(WS_POPUP | WS_CAPTION);
    style |= WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    SetWindowLongPtrA(hWnd, GWL_STYLE, style);

    SetWindowPos(hWnd, NULL, 100, 100, 1280, 720, SWP_FRAMECHANGED | SWP_SHOWWINDOW);
    g_WindowPatched = true;
}

static DWORD WINAPI AutoWindowThread(LPVOID)
{
    for (int tries = 0; tries < 60; ++tries)
    {
        ForceWindowedMode();
        if (g_WindowPatched) break;
        Sleep(500);
    }
    return 0;
}

DWORD WINAPI InitL4D2VR(HMODULE hModule)
{
// Allocate console for debug output in debug builds
#ifdef _DEBUG
    AllocConsole();
    FILE *fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
#endif

    std::cout << VER_PRODUCT " v" VER_VERSION " (" VER_DATE ")" << std::endl;
    std::cout << "Auto-detecting SteamVR... no launch options required." << std::endl;

    g_Game = new Game();

    // Auto-force windowed mode so no launch options are needed
    CreateThread(NULL, 0, AutoWindowThread, NULL, 0, NULL);

    return 0;
}



BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
            CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)InitL4D2VR, hModule, 0, NULL);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


