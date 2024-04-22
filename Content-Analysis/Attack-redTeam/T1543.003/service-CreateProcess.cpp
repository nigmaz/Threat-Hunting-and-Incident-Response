#include <Windows.h>
#include <iostream>
#include <string>

VOID WINAPI ServiceMain(DWORD argc, LPTSTR* argv);
VOID WINAPI ServiceCtrlHandler(DWORD dwCtrl);
DWORD WINAPI ServiceWorkerThread(LPVOID lpParam);

SERVICE_STATUS g_ServiceStatus;
SERVICE_STATUS_HANDLE g_ServiceStatusHandle;
HANDLE g_ServiceStopEvent = nullptr;

int main()
{
    SERVICE_TABLE_ENTRY ServiceTable[] =
    {
        { const_cast<LPWSTR>(L"WannaCryService"), (LPSERVICE_MAIN_FUNCTION)ServiceMain },
        { nullptr, nullptr }
    };

    if (!StartServiceCtrlDispatcher(ServiceTable))
    {
        return GetLastError();
    }

    return 0;
}

VOID WINAPI ServiceMain(DWORD argc, LPTSTR* argv)
{
    g_ServiceStatusHandle = RegisterServiceCtrlHandler(L"WannaCryService", ServiceCtrlHandler);
    if (!g_ServiceStatusHandle)
    {
        return;
    }

    g_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    g_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
    g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    g_ServiceStatus.dwWin32ExitCode = NO_ERROR;
    g_ServiceStatus.dwServiceSpecificExitCode = 0;
    g_ServiceStatus.dwCheckPoint = 0;

    if (!SetServiceStatus(g_ServiceStatusHandle, &g_ServiceStatus))
    {
        // Handle error
    }

    g_ServiceStopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!g_ServiceStopEvent)
    {
        // Handle error
        return;
    }

    HANDLE hThread = CreateThread(nullptr, 0, ServiceWorkerThread, nullptr, 0, nullptr);
    if (!hThread)
    {
        // Handle error
        CloseHandle(g_ServiceStopEvent);
        return;
    }

    WaitForSingleObject(hThread, INFINITE);

    CloseHandle(g_ServiceStopEvent);
}

VOID WINAPI ServiceCtrlHandler(DWORD dwCtrl)
{
    switch (dwCtrl)
    {
    case SERVICE_CONTROL_STOP:
        if (g_ServiceStatus.dwCurrentState != SERVICE_RUNNING)
            break;

        g_ServiceStatus.dwControlsAccepted = 0;
        g_ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
        g_ServiceStatus.dwWin32ExitCode = NO_ERROR;
        g_ServiceStatus.dwCheckPoint = 4;

        if (!SetServiceStatus(g_ServiceStatusHandle, &g_ServiceStatus))
        {
            // Handle error
        }

        SetEvent(g_ServiceStopEvent);
        WaitForSingleObject(g_ServiceStopEvent, INFINITE);

        g_ServiceStatus.dwControlsAccepted = 0;
        g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
        g_ServiceStatus.dwWin32ExitCode = NO_ERROR;
        g_ServiceStatus.dwCheckPoint = 5;

        if (!SetServiceStatus(g_ServiceStatusHandle, &g_ServiceStatus))
        {
            // Handle error
        }

        break;
    }
}

DWORD WINAPI ServiceWorkerThread(LPVOID lpParam)
{
    // Perform the main function of the service here
    // For this example, let's launch calc.exe

    std::wstring command = L"calc.exe"; // or PATH 
    STARTUPINFO startupInfo = {};
    PROCESS_INFORMATION processInfo = {};

    if (CreateProcess(nullptr, const_cast<LPWSTR>(command.c_str()), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &startupInfo, &processInfo))
    {
        WaitForSingleObject(processInfo.hProcess, INFINITE);
        CloseHandle(processInfo.hThread);
        CloseHandle(processInfo.hProcess);
    }

    SetEvent(g_ServiceStopEvent);

    return ERROR_SUCCESS;
}

