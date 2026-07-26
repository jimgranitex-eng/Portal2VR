#include "game.h"
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include "sdk.h"
#include "sounds.h"
#include "vr.h"
#include "hooks.h"
#include "offsets.h"
#include "sigscanner.h"
#include "log.h"

// Original Gistix launch options, applied in-engine (no Steam launch options required):
//   -insecure -window -novid
//   +mat_motion_blur_percent_of_screen_max 0 +mat_queue_mode 0 +mat_vsync 0
//   +mat_antialias 0 +mat_grain_scale_override 0 -width 1280 -height 720
// Portal 2 has no VAC; -insecure is informational only.
static const char* g_AutoExecCmds[] = {
    // Windowed + resolution (Gistix: -window -width 1280 -height 720)
    "mat_setvideomode 1280 720 1",
    "mat_viewportscale 1",
    // Gistix +mat_* launch options
    "mat_motion_blur_percent_of_screen_max 0",
    "mat_queue_mode 0",
    "mat_vsync 0",
    "mat_antialias 0",
    "mat_grain_scale_override 0",
    "mat_disable_bloom 1",
    "mat_hdr_level 0",
    // VR-friendly engine state
    "fog_enable 0",
    "r_drawmodelstatsoverlay 0",
    "r_shadows 0",
    "r_3dsky 0",
    "budget_show_history 0",
    "cl_showfps 0",
    "net_graph 0",
    "fps_max 0",
    "engine_no_focus_sleep 0",
};

static void ForceGameWindowed()
{
    HWND hwnd = FindWindowA("Valve001", nullptr);
    if (!hwnd)
        hwnd = FindWindowA(nullptr, "Portal 2");
    if (!hwnd)
        return;

    LONG style = GetWindowLongA(hwnd, GWL_STYLE);
    if (style & WS_POPUP)
    {
        style &= ~WS_POPUP;
        style |= WS_OVERLAPPEDWINDOW;
        SetWindowLongA(hwnd, GWL_STYLE, style);
    }
    ShowWindow(hwnd, SW_SHOWNORMAL);
    SetWindowPos(hwnd, HWND_TOP, 80, 40, 1280, 720, SWP_FRAMECHANGED | SWP_SHOWWINDOW);
}

static void RunAutoExecPass(Game* game, const char* passName)
{
    ForceGameWindowed();
    char msg[128];
    sprintf_s(msg, "AutoExec %s start (%d cmds)", passName,
        static_cast<int>(sizeof(g_AutoExecCmds) / sizeof(g_AutoExecCmds[0])));
    VRLog::Write(msg);

    for (int i = 0; i < static_cast<int>(sizeof(g_AutoExecCmds) / sizeof(g_AutoExecCmds[0])); ++i)
    {
        game->ClientCmd_Unrestricted(g_AutoExecCmds[i]);
        char buf[256];
        sprintf_s(buf, "AutoExec %s: %s", passName, g_AutoExecCmds[i]);
        VRLog::Write(buf);
    }
    VRLog::Write(passName);
}

static DWORD WINAPI AutoExecThread(LPVOID param)
{
    Game* game = static_cast<Game*>(param);
    VRLog::Write("AutoExecThread started \u2014 applying Gistix launch options in-engine");

    // Pass 1: soon after engine interfaces exist
    Sleep(1500);
    RunAutoExecPass(game, "pass1");

    // Pass 2: re-apply after video config may overwrite
    Sleep(2500);
    RunAutoExecPass(game, "pass2");

    // Pass 3: late re-assert critical VR window settings
    Sleep(4000);
    ForceGameWindowed();
    game->ClientCmd_Unrestricted("mat_setvideomode 1280 720 1");
    game->ClientCmd_Unrestricted("mat_vsync 0");
    game->ClientCmd_Unrestricted("mat_queue_mode 0");
    VRLog::Write("AutoExec pass3: reassert windowed + vsync/queue");
    VRLog::Write("AutoExecThread complete \u2014 no Steam launch options required");

    return 0;
}

Game::Game()
{
    VRLog::Write("Game constructor start");

    const int kMaxWaitMs = 60000; // 60 second timeout for DLL loading
    int waitMs = 0;

    while (!(m_BaseClient = reinterpret_cast<uintptr_t>(GetModuleHandle("client.dll"))))
    {
        Sleep(50);
        if ((waitMs += 50) > kMaxWaitMs) { errorMsg("Timed out waiting for client.dll"); return; }
    }
    waitMs = 0;
    VRLog::Write("client.dll loaded");
    while (!(m_BaseEngine = reinterpret_cast<uintptr_t>(GetModuleHandle("engine.dll"))))
    {
        Sleep(50);
        if ((waitMs += 50) > kMaxWaitMs) { errorMsg("Timed out waiting for engine.dll"); return; }
    }
    waitMs = 0;
    VRLog::Write("engine.dll loaded");
    while (!(m_BaseMaterialSystem = reinterpret_cast<uintptr_t>(GetModuleHandle("materialsystem.dll"))))
    {
        Sleep(50);
        if ((waitMs += 50) > kMaxWaitMs) { errorMsg("Timed out waiting for materialsystem.dll"); return; }
    }
    waitMs = 0;
    VRLog::Write("materialsystem.dll loaded");
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
    VRLog::Write("All DLLs loaded");

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
    VRLog::Write("Interfaces obtained");

    // Validate required interfaces
    if (!m_EngineClient)
    {
        errorMsg("Failed to obtain VEngineClient015 — VR cannot function");
        return;
    }
    if (!m_MaterialSystem)
    {
        errorMsg("Failed to obtain VMaterialSystem080 — rendering will fail");
        return;
    }
    if (!m_ClientEntityList)
        VRLog::Write("WARNING: VClientEntityList003 not obtained");
    if (!m_EngineTrace)
        VRLog::Write("WARNING: EngineTraceClient004 not obtained");
    if (!m_ModelInfo)
        VRLog::Write("WARNING: VModelInfoClient004 not obtained");
    if (!m_ModelRender)
        VRLog::Write("WARNING: VEngineModel016 not obtained");
    if (!m_VguiSurface)
        VRLog::Write("WARNING: VGUI_Surface031 not obtained");
    if (!m_EngineSound)
        VRLog::Write("WARNING: IEngineSound003 not obtained");

    m_Offsets = new Offsets();

    if (!m_Offsets->g_pClientMode.address)
    {
        errorMsg("g_pClientMode offset is invalid");
        return;
    }
    m_ClientMode = **(IClientMode***)(m_Offsets->g_pClientMode.address);

    VRLog::Write("Creating VR...");
    m_VR = new VR(this);
    VRLog::Write("Creating Hooks...");
    m_Hooks = new Hooks(this);

    m_Initialized = true;
    VRLog::Write("Game constructor complete: m_Initialized=1");

    // Auto-apply Gistix launch options + VR graphics — singleplayer and co-op
    VRLog::Write("Starting AutoExecThread (windowed + mat_* + VR defaults)");
    CreateThread(NULL, 0, AutoExecThread, this, 0, NULL);
}

Game::~Game()
{
    VRLog::Write("Game destructor start");
    m_Initialized = false;

    delete m_Hooks;
    m_Hooks = nullptr;

    if (m_VR)
    {
        m_VR->Shutdown();
        delete m_VR;
        m_VR = nullptr;
    }

    delete m_Offsets;
    m_Offsets = nullptr;

    VRLog::Write("Game destructor complete");
}

void *Game::GetInterface(const char *dllname, const char *interfacename)
{
    HMODULE hModule = GetModuleHandle(dllname);
    if (!hModule)
    {
        char buf[256];
        sprintf_s(buf, "GetInterface: %s not loaded (requested %s)", dllname, interfacename);
        VRLog::Write(buf);
        return nullptr;
    }

    auto CreateInterface = reinterpret_cast<tCreateInterface>(GetProcAddress(hModule, "CreateInterface"));
    if (!CreateInterface)
    {
        char buf[256];
        sprintf_s(buf, "GetInterface: CreateInterface not found in %s", dllname);
        VRLog::Write(buf);
        return nullptr;
    }

    int returnCode = 0;
    void *createdInterface = CreateInterface(interfacename, &returnCode);

    if (!createdInterface)
    {
        char buf[256];
        sprintf_s(buf, "GetInterface: %s returned null (return code %d) from %s", interfacename, returnCode, dllname);
        VRLog::Write(buf);
    }

    return createdInterface;
}

void Game::errorMsg(const char *msg)
{
    VRLog::Write(msg);
    MessageBoxA(0, msg, "Portal2VR", MB_ICONERROR | MB_OK);
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