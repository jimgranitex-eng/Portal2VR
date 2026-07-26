#pragma once
#include <Windows.h>
#include <string>
#include <fstream>
#include <filesystem>
#include <mutex>

class VRLog
{
public:
    static void Init();
    static void Write(const char* msg);
    static void Write(const std::string& msg);

private:
    static std::ofstream s_LogFile;
    static std::mutex s_Mutex;
    static bool s_Initialized;
    static std::filesystem::path s_LogPath;
};