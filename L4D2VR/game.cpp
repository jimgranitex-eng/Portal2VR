#include "game.h"
#include <Windows.h>
#include <iostream>
#include <fstream>
#include "sdk.h"
#include "sounds.h"
#include "vr.h"
#include "hooks.h"
#include "offsets.h"
#include "sigscanner.h"

static const char* g_AutoExecCmds[] = {
    "mat_motion_blur_percent_of_screen_max 0",
    "mat_queue_mode 0",
    "mat_vsync 0",
    "mat_antialias 0",
    "mat_grain_scale_override 0",
    "mat_disable_bloom 1",
    "fog_enable 0",
    "r_drawmodelstatsoverlay 0",
    "r_shadows 0",
    "budget_show_history 0",
    "cl_showfps 0",
    "net_graph 0",
};

static DWORD WINAPI AutoExecThread(LPVOID param)
{
    Game* game = static_cast<Game*>(param);
    Sleep(3000);
    for (int i = 0; i < sizeof(g_AutoExecCmds) / sizeof(g_AutoExecCmds[0]); ++i)
        game->ClientCmd_Unrestricted(g_AutoExecCmds[i]);
    return 0;
}

static void VRLog(const char* msg)
{
    char dir[MAX_STR_LEN];
    GetCurrentDirectory(MAX_STR_LEN, dir);
    char path[MAX_STR_LEN];
    sprintf_s(path, "%s\\VR\\portal2vr.log", dir);
    std::ofstream logFile(path, std::ios::app);
    if (logFile.is_open())
    {
        SYSTEMTIME st;
        GetLocalTime(&st);
        logFile << "[" << st.wHour << ":" << st.wMinute << ":" << st.wSecond
            << "." << st.wMilliseconds << "] [Game] " << msg << "\n";
    }
}

Game::Game()
{
    VRLog("Game constructor start");

    const int kMaxWaitMs = 60000; // 60 second timeout for DLL loading
    int waitMs = 0;

    while (!(m_BaseClient = reinterpret_cast<uintptr_t>(GetModuleHandle("client.dll"))))
    {
        Sleep(50);
        if ((waitMs += 50) > kMaxWaitMs) { errorMsg("Timed out waiting for client.dll"); return; }
    }
    waitMs = 0;
    VRLog("client.dll loaded");
    while (!(m_BaseEngine = reinterpret_cast<uintptr_t>(GetModuleHandle("engine.dll"))))
    {
        Sleep(50);
        if ((waitMs += 50) > kMaxWaitMs) { errorMsg("Timed out waiting for engine.dll"); return; }
    }
    waitMs = 0;
    VRLog("engine.dll loaded");
    while (!(m_BaseMaterialSystem = reinterpret_cast<uintptr_t>(GetModuleHandle("materialsystem.dll"))))
    {
        Sleep(50);
        if ((waitMs += 50) > kMaxWaitMs) { errorMsg("Timed out waiting for materialsystem.dll"); return; }
    }
    waitMs = 0;
    VRLog("materialsystem.dll loaded");
    while (!(m_BaseServer = reinterpret_cast<uintptr_t>(GetModuleHandle("server.dll"))))
    {
        Sleep(50);
        if ((waitMs += 50) > kMaxWaitMs) { errorMsg("Timed out waiting for server.dll"); return; }
    }
    waitMs = 0;
    while (!(m_BaseVgui2 = reinterpret_cast<uintptr_t>(GetModuleHandle("vgui2.dll"))))
    {
        Sleep(50);
        if ((waitMs += 50) > kMaxWaitMs) { errorMsg("Timed out waiting for vgui2.dll"); return; }
    }
    VRLog("All DLLs loaded");

    m_ClientEntityList = reinterpret_cast<IClientEntityList *>(GetInterface("client.dll", "VClientEntityList003"));
    m_EngineTrace = reinterpret_cast<IEngineTrace *>(GetInterface("engine.dll", "EngineTraceClient004"));
    m_EngineClient = reinterpret_cast<IEngineClient *>(GetInterface("engine.dll", "VEngineClient015"));
    m_MaterialSystem = reinterpret_cast<IMaterialSystem *>(GetInterface("materialsystem.dll", "VMaterialSystem080"));
    m_ClientViewRender = reinterpret_cast<IViewRender *>(GetInterface("client.dll", "VEngineRenderView013"));
    m_EngineViewRender = reinterpret_cast<IViewRender *>(GetInterface("engine.dll", "VEngineRenderView013"));
    m_ModelInfo = reinterpret_cast<IModelInfo *>(GetInterface("engine.dll", "VModelInfoClient004"));
    m_ModelRender = reinterpret_cast<IModelRender *>(GetInterface("engine.dll", "VEngineModel016"));
    m_VguiInput = reinterpret_cast<IInput *>(GetInterface("vgui2.dll", "VGUI_InputInternal001"));
    m_VguiSurface = reinterpret_cast<ISurface *>(GetInterface("vguimatsurface.dll", "VGUI_Surface031"));
    m_EngineSound = reinterpret_cast<IEngineSound *>(GetInterface("engine.dll", "IEngineSound003"));
    VRLog("Interfaces obtained");

    m_Offsets = new Offsets();

    m_ClientMode = **(IClientMode***)(m_Offsets->g_pClientMode.address);

    VRLog("Creating VR...");
    m_VR = new VR(this);
    VRLog("Creating Hooks...");
    m_Hooks = new Hooks(this);

    m_Initialized = true;
    VRLog("Game constructor complete: m_Initialized=1");

    // Auto-apply optimal settings — no launch options needed
    CreateThread(NULL, 0, AutoExecThread, this, 0, NULL);
}

void *Game::GetInterface(const char *dllname, const char *interfacename)
{
    HMODULE hModule = GetModuleHandle(dllname);
    if (!hModule)
        return nullptr;

    auto CreateInterface = reinterpret_cast<tCreateInterface>(GetProcAddress(hModule, "CreateInterface"));
    if (!CreateInterface)
        return nullptr;

    int returnCode = 0;
    void *createdInterface = CreateInterface(interfacename, &returnCode);

    return createdInterface;
}

void Game::errorMsg(const char *msg)
{
    MessageBox(0, msg, "L4D2VR", MB_ICONERROR | MB_OK);
}

CBaseEntity *Game::GetClientEntity(int entityIndex)
{
    return reinterpret_cast<CBaseEntity *>(m_ClientEntityList->GetClientEntity(entityIndex));
}

char *Game::getNetworkName(uintptr_t *entity)
{
    auto *IClientNetworkableVtable = reinterpret_cast<uintptr_t *>(*(entity + 0x8));
    auto *GetClientClassPtr = reinterpret_cast<uintptr_t *>(*(IClientNetworkableVtable + 0x8));
    auto *ClientClassPtr = reinterpret_cast<uintptr_t *>(*(GetClientClassPtr + 0x1));
    auto *m_pNetworkName = reinterpret_cast<char *>(*(ClientClassPtr + 0x8));
    int classID = static_cast<int>(*(ClientClassPtr + 0x10));
    std::cout << "ClassID: " << classID << std::endl;
    return m_pNetworkName;
}

void Game::ClientCmd(const char *szCmdString)
{
    m_EngineClient->ClientCmd(szCmdString);
}

void Game::ClientCmd_Unrestricted(const char *szCmdString)
{
    m_EngineClient->ClientCmd_Unrestricted(szCmdString);
}


