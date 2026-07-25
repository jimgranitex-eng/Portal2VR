// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>
#include <iostream>
#include <fstream>
#include "game.h"
#include "hooks.h"
#include "vr.h"
#include "sdk.h"

#define VER_PRODUCT "Portal 2 VR Mod"
#define VER_VERSION "5.3.0.7"
#define VER_DATE    "2026-07-24T22:30:00Z"

static void VRLog(const char* msg)
{
    char exePath[MAX_PATH];
    GetModuleFileName(NULL, exePath, MAX_PATH);
    char* lastSlash = strrchr(exePath, '\\');
    if (lastSlash) *lastSlash = '\0';
    char path[MAX_PATH];
    sprintf_s(path, "%s\\VR\\portal2vr.log", exePath);
    std::ofstream log(path, std::ios::app);
    if (log.is_open())
    {
        SYSTEMTIME st;
        GetLocalTime(&st);
        log << "[" << st.wHour << ":" << st.wMinute << ":" << st.wSecond
            << "." << st.wMilliseconds << "] " << msg << "\n";
    }
}

DWORD WINAPI InitL4D2VR(HMODULE hModule)
{
#ifdef _DEBUG
    AllocConsole();
    FILE *fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
#endif

    VRLog("InitL4D2VR thread started");
    g_Game = new Game();
    VRLog("InitL4D2VR complete");

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
            VRLog("DLL_PROCESS_ATTACH");
            DisableThreadLibraryCalls(hModule);
            CreateThread(NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(InitL4D2VR), hModule, 0, NULL);
            break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


