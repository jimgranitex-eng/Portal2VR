#include "log.h"
#include <iostream>
#include <chrono>
#include <ctime>

std::ofstream VRLog::s_LogFile;
std::mutex VRLog::s_Mutex;
bool VRLog::s_Initialized = false;
std::filesystem::path VRLog::s_LogPath;

void VRLog::Init()
{
    std::lock_guard<std::mutex> lock(s_Mutex);

    if (s_Initialized)
        return;

    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);
    s_LogPath = std::filesystem::path(exePath).parent_path() / "VR" / "portal2vr.log";

    std::error_code ec;
    std::filesystem::create_directories(s_LogPath.parent_path(), ec);

    s_LogFile.open(s_LogPath, std::ios::app);
    if (s_LogFile.is_open())
    {
        s_Initialized = true;

        SYSTEMTIME st;
        GetLocalTime(&st);
        s_LogFile << "[" << st.wHour << ":" << st.wMinute << ":" << st.wSecond
            << "." << st.wMilliseconds << "] [Log Init] Log file opened: "
            << s_LogPath.string() << "\n";
        s_LogFile.flush();
    }
}

void VRLog::Write(const char* msg)
{
    if (!s_Initialized)
        Init();

    std::lock_guard<std::mutex> lock(s_Mutex);
    if (s_LogFile.is_open())
    {
        SYSTEMTIME st;
        GetLocalTime(&st);
        s_LogFile << "[" << st.wHour << ":" << st.wMinute << ":" << st.wSecond
            << "." << st.wMilliseconds << "] " << msg << "\n";
        s_LogFile.flush();
    }
}

void VRLog::Write(const std::string& msg)
{
    Write(msg.c_str());
}