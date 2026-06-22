#pragma once
#include <windows.h>
#include <string>

void Log(const std::wstring &msg);
void CheckExplorer();
void CheckRegistry();
void CheckProcesses();
void DoCheck();