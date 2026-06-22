#include "common.h"

void Log(const char *msg)
{
	std::ofstream log(LOG_FILE, std::ios::app);
	if (log.is_open())
	{
		log << "[" << time(NULL) << "] " << msg << std::endl;
		log.close();
	}
}

std::string GetDllPath()
{
	return std::string(INSTALL_PATH) + DLL_NAME;
}

std::string GetLoaderPath()
{
	return std::string(INSTALL_PATH) + LOADER_NAME;
}

bool FileExists(const std::string &path)
{
	return GetFileAttributesA(path.c_str()) != INVALID_FILE_ATTRIBUTES;
}

DWORD GetPidByName(const wchar_t *name)
{
	PROCESSENTRY32W entry = {sizeof(PROCESSENTRY32W)};
	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snap == INVALID_HANDLE_VALUE)
		return 0;

	if (Process32FirstW(snap, &entry))
	{
		do
		{
			if (_wcsicmp(entry.szExeFile, name) == 0)
			{
				CloseHandle(snap);
				return entry.th32ProcessID;
			}
		} while (Process32NextW(snap, &entry));
	}
	CloseHandle(snap);
	return 0;
}

bool InjectDll(DWORD pid, const std::string &dllPath)
{
	HANDLE hProcess = OpenProcess(
		PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE,
		FALSE, pid);
	if (!hProcess)
		return false;

	size_t len = dllPath.length() + 1;
	LPVOID pRemote = VirtualAllocEx(hProcess, NULL, len, MEM_COMMIT, PAGE_READWRITE);
	if (!pRemote)
	{
		CloseHandle(hProcess);
		return false;
	}

	WriteProcessMemory(hProcess, pRemote, dllPath.c_str(), len, NULL);

	FARPROC loadLib = GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)loadLib, pRemote, 0, NULL);

	if (hThread)
	{
		WaitForSingleObject(hThread, INFINITE);
		CloseHandle(hThread);
		CloseHandle(hProcess);
		return true;
	}

	CloseHandle(hProcess);
	return false;
}