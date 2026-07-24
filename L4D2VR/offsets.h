#pragma once
#include "sigscanner.h"
#include "game.h"


struct Offset
{
    std::string moduleName;
    int offset;
    int address;
    std::string signature;
    int sigOffset;

    Offset(std::string moduleName, int currentOffset, std::string signature, int sigOffset = 0)
    {
        this->moduleName = moduleName;
        this->offset = currentOffset;
        this->signature = signature;
        this->sigOffset = sigOffset;

        int newOffset = SigScanner::VerifyOffset(moduleName, currentOffset, signature, sigOffset);
        if (newOffset > 0)
        {
            this->offset = newOffset;
        }
            
        if (newOffset == -1)
        {
            Game::errorMsg(("Signature not found: " + signature).c_str());
            return;
        }

        this->address = (uintptr_t)GetModuleHandle(moduleName.c_str()) + this->offset;
    }
};

class Offsets
{
public:
    Offset RenderView =                  { "client.dll", 0x1F2120, "55 8B EC 83 EC 2C 53 56 8B F1 6A 00 8D 8E ? ? ? ? E8 ? ? ? ?" };
    Offset g_pClientMode =               { "client.dll", 0x28A600, "8B 0D ? ? ? ? 8B", 2 };
    Offset CalcViewModelView =           { "client.dll", 0x27D750, "55 8B EC 83 EC 34 53 8B D9 80 BB" };
    Offset CreateMove =                  { "client.dll", 0x27A440, "55 8B EC A1 ? ? ? ? 83 EC 0C 83 78 30 00 56 8B 75 0C 57 8B F9 74 43" };

    Offset WriteUsercmd =                { "client.dll", 0x1C2060, "55 8B EC A1 ? ? ? ? 83 78 30 00 53 8B 5D 0C 56 57" };
    Offset PrePushRenderTarget =         { "client.dll", 0xA8C80, "55 8B EC 8B C1 56 8B 75 08 8B 0E 89 08 8B 56 04 89" };

    Offset ReadUserCmd =                 { "server.dll", 0x205100, "55 8B EC 53 8B 5D 10 56 57 8B 7D 0C 53" };
    Offset ProcessUsercmds =             { "server.dll", 0x170300, "55 8B EC B8 ? ? ? ? E8 ? ? ? ? 0F 57 C0 53 56 57 B9 ? ? ? ? 8D 85 ? ? ? ? 33 DB" }; //?
    Offset CBaseEntity_entindex =        { "server.dll", 0x39F00, "8B 41 1C 85 C0 75 01 C3 8B 0D ? ? ? ? 2B 41 58 C1 F8 04 C3 CC"};
    Offset EyePosition =                 { "server.dll", 0xF40E0, "55 8B EC 56 8B F1 8B 86 ? ? ? ? C1 E8 0B A8 01 74 05 E8 ? ? ? ? 8B 45 08 F3" };

    Offset PushRenderTargetAndViewport = { "materialsystem.dll", 0x2D5F0, "55 8B EC 83 EC 24 8B 45 08 8B 55 10 89" };
    Offset PopRenderTargetAndViewport =  { "materialsystem.dll", 0x2CE80, "56 8B F1 83 7E 4C 00" };

    // Firing Portals
    Offset TraceFirePortalServer =       { "server.dll", 0x400D50, "53 8B DC 83 EC 08 83 E4 F0 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 81 EC ? ? ? ? 56 57 8B F1 6A" };

    Offset DrawModelExecute =            { "engine.dll", 0xE05E0, "55 8B EC 81 EC ? ? ? ? A1 ? ? ? ? 33 C5 89 45 FC 8B 45 10 56 8B 75 08 57 8B" };
    Offset VGui_Paint =                  { "engine.dll", 0x115CE0, "55 8B EC E8 ? ? ? ? 8B 10 8B C8 8B 52 38" };

    Offset PlayerPortalled = { "client.dll", 0x27C9D0, "55 8B EC 83 EC 78 53 56 8B D9 8B 0D ? ? ? ? 8B 01 8B 90 ? ? ? ? 57 33 FF 57 FF D2" };

    // Ingame UI
    Offset DrawSelf = { "client.dll", 0x12CC90, "55 8B EC 56 8B F1 80 BE ? ? ? ? ? 0F 84 ? ? ? ? 8B 0D" };
    Offset ClipTransform = { "client.dll", 0x1DD130, "55 8B EC 8B 0D ? ? ? ? 8B 01 8B 90 ? ? ? ? FF D2 8B 4D" };
    // Pointer laser
    Offset CreatePingPointer = { "client.dll", 0x280660, "55 8B EC 83 EC 14 53 56 8B F1 8B 8E ? ? ? ? 57 85 C9 74 30" };
    Offset GetPortalPlayer = { "client.dll", 0x8DCA0, "55 8B EC 8B 45 08 83 F8 FF 75 10 8B 0D ? ? ? ? 8B 01 8B 90 ? ? ? ? FF D2" };
    Offset PrecacheParticleSystem = { "server.dll", 0x16DF40, "55 8B EC 8B 0D ? ? ? ? 8B 55 08 8B 01 8B 40 20 6A 00 6A FF" };
    Offset Precache = { "server.dll", 0x35A2C0, "E8 ? ? ? ? 68 ? ? ? ? E8 ? ? ? ?" };
    Offset SetControlPoint = { "client.dll", 0x17BD30, "55 8B EC 53 56 8B 75 0C 57 8B F9 BB ? ? ? ? 84 9F ? ? ? ?" };
    Offset SetDrawOnlyForSplitScreenUser = { "client.dll", 0x17B9E0, "55 8B EC 8B 45 08 53 8B D9 3B 83 ? ? ? ? 74 55" };
    Offset StopEmission = { "client.dll", 0x17B6A0, "55 8B EC 53 8B 5D 08 57 8B F9 F6 87 ? ? ? ? ? 74 7F" };

    // Aim related
    Offset CHudCrosshair_ShouldDraw = { "client.dll", 0x141BE0, "57 8B F9 80 BF ? ? ? ? ? 74 04 32 C0 5F C3" };

    // VR Eyes
    Offset UTIL_Portal_FirstAlongRay = { "server.dll", 0x377200, "55 8B EC 8B 0D ? ? ? ? 85 C9 74 19 A1 ? ? ? ?" };
    Offset UTIL_IntersectRayWithPortal = { "server.dll", 0x376730, "55 8B EC 83 EC 48 56 8B 75 0C 85 F6 0F 84 ? ? ? ?" };
    Offset UTIL_Portal_AngleTransform = { "server.dll", 0x375CA0, "55 8B EC 8B 45 08 8B 4D 0C 83 EC 0C 50 51 8D 55 F4" };

    //Grababbles
    Offset Weapon_ShootPosition = { "server.dll", 0x1033C0, "55 8B EC 8B 01 8B 90 ? ? ? ? 56 8B 75 08 56 FF D2 8B C6 5E 5D C2 04 00" };
    Offset ComputeError = { "server.dll", 0x3C8140, "53 8B DC 83 EC 08 83 E4 F0 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 81 EC ? ? ? ? 56 8B F1 8B 86 ? ? ? ? 57 83 F8 FF 74 2A" };
    Offset UpdateObject = { "server.dll", 0x3CA010, "53 8B DC 83 EC 08 83 E4 F0 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 81 EC ? ? ? ? 56 57 8B F9 8B 87 ? ? ? ? 89 BD" };
    Offset UpdateObjectVM = { "server.dll", 0x3CBB10, "53 8B DC 83 EC 08 83 E4 F0 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 81 EC ? ? ? ? 56 57 8B F9 8B 87 ? ? ? ? 83 F8" };
    Offset RotateObject = { "server.dll", 0x3C7890, "55 8B EC 0F 57 C0 F3 0F 10 4D ? 81 EC ? ? ? ? 0F 2E C8 9F 57 8B F9 F6 C4 44 7A 12" };
    Offset EyeAngles = { "server.dll", 0x103A50, "55 8B EC 8B 81 ? ? ? ? 83 EC 60 56 57 8B 3D ? ? ? ? 83 F8 FF 74 1D" };

    Offset GetFOV = { "client.dll", 0x2772B0, "55 8B EC 51 56 8B F1 E8 ? ? ? ? D9 5D FC 8B 06 8B 90 ? ? ? ? 8B CE FF D2" };
    Offset GetDefaultFOV = { "client.dll", 0x279020, "A1 ? ? ? ? F3 0F 2C 40 ? C3" };
    Offset GetViewModelFOV = { "client.dll", 0x28AB80, "A1 ? ? ? ? D9 40 2C C3" };

    // Multiplayer
    Offset GetOwner = { "server.dll", 0xD7550, "8B 81 ? ? ? ? 83 F8 FF 74 23 8B 15 ? ? ? ?" };

    // Sound
    Offset S_UpdateListener =             { "engine.dll", 0x1A3F50, "55 8B EC 83 E4 F8 81 EC B4 00 00 00 53 56 8B 35 ? ? ? ? 57 8B F9" };
};