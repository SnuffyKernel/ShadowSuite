#include "guard.h"
#include <psapi.h>
#include <tlhelp32.h>
#include <fstream>

#pragma comment(lib, "psapi.lib")

#define LOG_FILE L"C:\\ProgramData\\ShadowGuard\\guard.log"

void Log(const std::wstring &msg)
{
	std::wofstream log(LOG_FILE, std::ios::app);
	if (log.is_open())
	{
		SYSTEMTIME st;
		GetLocalTime(&st);
		log << L"[" << st.wHour << L":" << st.wMinute << L":" << st.wSecond
			<< L"] " << msg << std::endl;
		log.close();
	}
}

void CheckExplorer()
{
	DWORD pid = 0;
	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snap)
	{
		PROCESSENTRY32W pe = {sizeof(PROCESSENTRY32W)};
		if (Process32FirstW(snap, &pe))
		{
			do
			{
				if (wcscmp(pe.szExeFile, L"explorer.exe") == 0)
				{
					pid = pe.th32ProcessID;
					break;
				}
			} while (Process32NextW(snap, &pe));
		}
		CloseHandle(snap);
	}

	if (!pid)
		return;

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
	if (!hProcess)
		return;

	HMODULE hMods[1024];
	DWORD cb;
	bool found = false;

	if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cb))
	{
		int count = cb / sizeof(HMODULE);
		for (int i = 0; i < count; i++)
		{
			wchar_t path[MAX_PATH];
			if (GetModuleFileNameExW(hProcess, hMods[i], path, MAX_PATH))
			{
				std::wstring dll = path;
				if (dll.find(L"beacon.dll") != std::wstring::npos ||
					dll.find(L"test.dll") != std::wstring::npos ||
					dll.find(L"payload.dll") != std::wstring::npos)
				{
					Log(L"[!] Found: " + dll);
					found = true;
				}
			}
		}
	}
	CloseHandle(hProcess);

	if (found)
	{
		Log(L"[!] Killing explorer.exe...");
		HANDLE hProc = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
		if (hProc)
		{
			TerminateProcess(hProc, 0);
			CloseHandle(hProc);
		}
		Sleep(2000);
		STARTUPINFOW si = {sizeof(STARTUPINFOW)};
		PROCESS_INFORMATION pi;
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_SHOW;
		CreateProcessW(L"C:\\Windows\\explorer.exe", NULL, NULL, NULL,
					   FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
		if (pi.hProcess)
			CloseHandle(pi.hProcess);
		if (pi.hThread)
			CloseHandle(pi.hThread);
		Log(L"[+] Explorer restarted");
	}
}

void CheckRegistry()
{
	HKEY hKey;
	if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
					  L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon",
					  0, KEY_READ | KEY_SET_VALUE, &hKey) == ERROR_SUCCESS)
	{

		wchar_t buf[4096] = {0};
		DWORD size = sizeof(buf);
		DWORD type = REG_SZ;

		if (RegQueryValueExW(hKey, L"Userinit", NULL, &type, (BYTE *)buf, &size) == ERROR_SUCCESS)
		{
			std::wstring val(buf);
			if (val.find(L"loader.exe") != std::wstring::npos ||
				val.find(L"beacon") != std::wstring::npos ||
				val.find(L"injector") != std::wstring::npos)
			{
				Log(L"[!] Userinit: " + val);
				std::wstring restore = L"C:\\Windows\\system32\\userinit.exe";
				RegSetValueExW(hKey, L"Userinit", 0, REG_SZ,
							   (BYTE *)restore.c_str(), (restore.size() + 1) * sizeof(wchar_t));
				Log(L"[+] Registry restored");
			}
		}
		RegCloseKey(hKey);
	}
}

void CheckProcesses()
{
	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (!snap)
		return;

	PROCESSENTRY32W pe = {sizeof(PROCESSENTRY32W)};
	if (Process32FirstW(snap, &pe))
	{
		do
		{
			std::wstring name = pe.szExeFile;
			if (name.find(L"loader.exe") != std::wstring::npos ||
				name.find(L"injector.exe") != std::wstring::npos ||
				name.find(L"beacon_loader.exe") != std::wstring::npos ||
				name.find(L"beacon.exe") != std::wstring::npos)
			{
				Log(L"[!] Process: " + name + L" PID: " + std::to_wstring(pe.th32ProcessID));
				HANDLE hProc = OpenProcess(PROCESS_TERMINATE, FALSE, pe.th32ProcessID);
				if (hProc)
				{
					TerminateProcess(hProc, 0);
					CloseHandle(hProc);
					Log(L"[+] Killed: " + name);
				}
			}
		} while (Process32NextW(snap, &pe));
	}
	CloseHandle(snap);
}

void DoCheck()
{
	CheckRegistry();
	CheckProcesses();
	CheckExplorer();
}