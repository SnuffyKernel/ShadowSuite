#pragma once
#include <windows.h>

void InstallService();
void UninstallService();
void RunAsService();
void WINAPI ServiceMain(DWORD argc, LPWSTR *argv);
void WINAPI ServiceCtrlHandler(DWORD control);