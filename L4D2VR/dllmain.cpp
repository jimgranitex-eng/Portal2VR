// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>
#include <iostream>
#include "game.h"
#include "hooks.h"
#include "vr.h"
#include "sdk.h"

#define VER_PRODUCT "Portal 2 VR Mod"
#define VER_VERSION "5.3.0.5"
#define VER_DATE    "2026-07-24T16:30:00Z"

static bool g_WindowPatched = false;
static HWINEVENTHOOK g_WinEventHook = nullptr;

static LONG WINAPI CrashHandler(EXCEPTION_POINTERS* ex)
{
    std::cerr << "FATAL: Portal2VR crashed at 0x" << std::hex
        << ex->ExceptionRecord->ExceptionAddress
        << " (code 0x" << ex->ExceptionRecord->ExceptionCode << ")" << std::endl;
    return EXCEPTION_CONTINUE_SEARCH;
}

static void CALLBACK WindowCreatedHook(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd,
    LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime)
{
    if (g_WindowPatched)
        return;

    char className[256];
    if (!GetClassNameA(hwnd, className, sizeof(className)))
        return;

    if (strcmp(className, "Valve001") != 0)
        return;

    LONG style = GetWindowLongPtrA(hwnd, GWL_STYLE);
    style &= ~(WS_POPUP | WS_CAPTION);
    style |= WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    SetWindowLongPtrA(hwnd, GWL_STYLE, style);

    SetWindowPos(hwnd, NULL, 100, 100, 1280, 720, SWP_FRAMECHANGED | SWP_SHOWWINDOW);
    g_WindowPatched = true;
}

DWORD WINAPI InitL4D2VR(HMODULE hModule)
{
// Allocate console for debug output in debug builds
#ifdef _DEBUG
    AllocConsole();
    FILE *fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
#endif

    SetUnhandledExceptionFilter(CrashHandler);

    std::cout << VER_PRODUCT " v" VER_VERSION " (" VER_DATE ")" << std::endl;
    std::cout << "Auto-detecting SteamVR... no launch options required." << std::endl;

    g_Game = new Game();

    // Auto-force windowed mode via event hook instead of polling
    g_WinEventHook = SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_CREATE,
        NULL, WindowCreatedHook, 0, 0, WINEVENT_OUTOFCONTEXT);

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
            DisableThreadLibraryCalls(hModule);
            CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)InitL4D2VR, hModule, 0, NULL);
            break;
    case DLL_PROCESS_DETACH:
        if (g_WinEventHook)
        {
            UnhookWinEvent(g_WinEventHook);
            g_WinEventHook = nullptr;
        }
        if (g_Game)
        {
            if (g_Game->m_VR)
                g_Game->m_VR->Shutdown();
            delete g_Game;
            g_Game = nullptr;
        }
        SetUnhandledExceptionFilter(nullptr);
        break;
    }
    return TRUE;
}


