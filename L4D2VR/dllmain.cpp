// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>
#include <iostream>
#include "game.h"
#include "hooks.h"
#include "vr.h"
#include "sdk.h"
#include "log.h"

#define VER_PRODUCT "Portal 2 VR Mod"
#define VER_VERSION "5.3.0.9"
#define VER_DATE    "2026-07-26T01:03:00Z"

DWORD WINAPI InitL4D2VR(HMODULE hModule)
{
#ifdef _DEBUG
    AllocConsole();
    FILE *fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
#endif

    VRLog::Write("InitL4D2VR thread started");
    g_Game = new Game();
    VRLog::Write("InitL4D2VR complete");

    return 0;
}

// Cleanup handler for DLL detach
void CleanupL4D2VR()
{
    if (g_Game)
    {
        delete g_Game;
        g_Game = nullptr;
        VRLog::Write("Cleanup: Game object deleted");
    }
}

// Callback for QueueUserAPC — runs on the main thread when it enters alertable state
void CALLBACK InitAPCProc(ULONG_PTR dwParam)
{
    InitL4D2VR(reinterpret_cast<HMODULE>(dwParam));
}

BOOL APIENTRY DllMain(HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        // Loader-lock safe: only DisableThreadLibraryCalls + QueueUserAPC.
        // All CRT / logging / VR init runs on the main thread via APC callback.
        DisableThreadLibraryCalls(hModule);

        // QueueUserAPC is safe in DllMain because it does not create a new thread;
        // it queues a callback to the existing process's main thread when it
        // enters an alertable wait state (e.g. SleepEx, WaitForSingleObjectEx).
        // This avoids the loader lock deadlock risk of CreateThread.
        if (!QueueUserAPC(InitAPCProc, GetCurrentThread(), reinterpret_cast<ULONG_PTR>(hModule)))
        {
            // Fallback: if QueueUserAPC fails, use CreateThread as last resort
            CreateThread(NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(InitL4D2VR), hModule, 0, NULL);
        }
        break;
    case DLL_PROCESS_DETACH:
        CleanupL4D2VR();
        break;
    default:
        break;
    }
    return TRUE;
}