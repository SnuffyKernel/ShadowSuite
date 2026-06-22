#include "service.h"
#include "guard.h"
#include <string>

#define SERVICE_NAME L"ShadowGuard"

SERVICE_STATUS g_status;
SERVICE_STATUS_HANDLE g_handle;
bool g_running = true;

void WINAPI ServiceCtrlHandler(DWORD control)
{
	if (control == SERVICE_CONTROL_STOP || control == SERVICE_CONTROL_SHUTDOWN)
	{
		g_running = false;
		g_status.dwCurrentState = SERVICE_STOPPED;
		SetServiceStatus(g_handle, &g_status);
	}
}

void WINAPI ServiceMain(DWORD argc, LPWSTR *argv)
{
	g_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	g_status.dwCurrentState = SERVICE_START_PENDING;
	g_status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;

	g_handle = RegisterServiceCtrlHandlerW(SERVICE_NAME, ServiceCtrlHandler);
	if (!g_handle)
		return;

	CreateDirectoryW(L"C:\\ProgramData\\ShadowGuard", NULL);
	Log(L"[SERVICE] Started");

	g_status.dwCurrentState = SERVICE_RUNNING;
	SetServiceStatus(g_handle, &g_status);

	while (g_running)
	{
		DoCheck();
		Sleep(5000);
	}

	g_status.dwCurrentState = SERVICE_STOPPED;
	SetServiceStatus(g_handle, &g_status);
}

void InstallService()
{
	SC_HANDLE scm = OpenSCManagerW(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if (!scm)
	{
		MessageBoxW(NULL, L"OpenSCManager failed", L"ShadowGuard", MB_OK);
		return;
	}

	wchar_t path[MAX_PATH];
	GetModuleFileNameW(NULL, path, MAX_PATH);
	std::wstring full = std::wstring(path) + L" --service";

	SC_HANDLE svc = CreateServiceW(scm, SERVICE_NAME, L"ShadowGuard",
								   SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START,
								   SERVICE_ERROR_NORMAL, full.c_str(), NULL, NULL, NULL, NULL, NULL);

	if (svc)
	{
		CloseServiceHandle(svc);
		MessageBoxW(NULL, L"Service installed!\nRun: sc start ShadowGuard", L"ShadowGuard", MB_OK);
	}
	else
	{
		if (GetLastError() == ERROR_SERVICE_EXISTS)
		{
			MessageBoxW(NULL, L"Service already exists", L"ShadowGuard", MB_OK);
		}
		else
		{
			std::wstring error = L"Install failed: " + std::to_wstring(GetLastError());
			MessageBoxW(NULL, error.c_str(), L"ShadowGuard", MB_OK);
		}
	}
	CloseServiceHandle(scm);
}

void UninstallService()
{
	SC_HANDLE scm = OpenSCManagerW(NULL, NULL, SC_MANAGER_CONNECT);
	if (!scm)
		return;

	SC_HANDLE svc = OpenServiceW(scm, SERVICE_NAME, DELETE);
	if (svc)
	{
		SERVICE_STATUS status;
		ControlService(svc, SERVICE_CONTROL_STOP, &status);
		DeleteService(svc);
		CloseServiceHandle(svc);
		MessageBoxW(NULL, L"Service removed", L"ShadowGuard", MB_OK);
	}
	else
	{
		MessageBoxW(NULL, L"Service not found", L"ShadowGuard", MB_OK);
	}
	CloseServiceHandle(scm);
}

void RunAsService()
{
	SERVICE_TABLE_ENTRYW table[] = {
		{(LPWSTR)SERVICE_NAME, (LPSERVICE_MAIN_FUNCTIONW)ServiceMain},
		{NULL, NULL}};
	StartServiceCtrlDispatcherW(table);
}