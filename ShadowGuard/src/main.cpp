#include "service.h"
#include "guard.h"
#include <iostream>

int main(int argc, char *argv[])
{
	BOOL isAdmin = FALSE;
	HANDLE hToken = NULL;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
	{
		TOKEN_ELEVATION e = {};
		DWORD sz = 0;
		if (GetTokenInformation(hToken, TokenElevation, &e, sizeof(e), &sz))
		{
			isAdmin = e.TokenIsElevated;
		}
		CloseHandle(hToken);
	}

	if (!isAdmin)
	{
		MessageBoxW(NULL, L"Run as Administrator!", L"ShadowGuard", MB_OK);
		return 1;
	}

	if (argc > 1)
	{
		std::string arg = argv[1];
		if (arg == "--install" || arg == "-i")
		{
			InstallService();
			return 0;
		}
		if (arg == "--uninstall" || arg == "-u")
		{
			UninstallService();
			return 0;
		}
		if (arg == "--service" || arg == "-s")
		{
			RunAsService();
			return 0;
		}
		if (arg == "--help" || arg == "-h")
		{
			std::wcout << L"ShadowGuard" << std::endl;
			std::wcout << L"  --install    Install service" << std::endl;
			std::wcout << L"  --uninstall  Uninstall service" << std::endl;
			return 0;
		}
	}

	std::wcout << L"[*] ShadowGuard" << std::endl;
	Log(L"[*] Started");

	while (true)
	{
		DoCheck();
		Sleep(5000);
	}
	
	return 0;
}
