#include <Windows.h>
#include <iostream>
#include <string>

// Function prototypes
VOID WINAPI ServiceMain(DWORD argc, LPTSTR* argv);
VOID WINAPI ServiceCtrlHandler(DWORD dwCtrl);
DWORD WINAPI ServiceWorkerThread(LPVOID lpParam);

// Global variables
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
        // Error handling
        return GetLastError();
    }

    return 0;
}

VOID WINAPI ServiceMain(DWORD argc, LPTSTR* argv)
{
    g_ServiceStatusHandle = RegisterServiceCtrlHandler(L"WannaCryService", ServiceCtrlHandler);
    if (!g_ServiceStatusHandle)
    {
        // Error handling
        return;
    }

    // The service is starting
    g_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    g_ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
    g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    g_ServiceStatus.dwWin32ExitCode = NO_ERROR;
    g_ServiceStatus.dwServiceSpecificExitCode = 0;
    g_ServiceStatus.dwCheckPoint = 0;

    if (!SetServiceStatus(g_ServiceStatusHandle, &g_ServiceStatus))
    {
        // Error handling
    }

    // Perform tasks here while the service is starting
    // For this example, we'll just display a "Hello World" message

    g_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
    if (!SetServiceStatus(g_ServiceStatusHandle, &g_ServiceStatus))
    {
        // Error handling
    }

    // Start a worker thread to perform the service's main function
    g_ServiceStopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!g_ServiceStopEvent)
    {
        // Error handling
        return;
    }

    HANDLE hThread = CreateThread(nullptr, 0, ServiceWorkerThread, nullptr, 0, nullptr);
    if (!hThread)
    {
        // Error handling
        CloseHandle(g_ServiceStopEvent);
        return;
    }

    // Wait until the worker thread completes, or the service is stopped
    WaitForSingleObject(hThread, INFINITE);

    // Cleanup
    CloseHandle(g_ServiceStopEvent);
}

VOID WINAPI ServiceCtrlHandler(DWORD dwCtrl)
{
    switch (dwCtrl)
    {
    case SERVICE_CONTROL_STOP:
        if (g_ServiceStatus.dwCurrentState != SERVICE_RUNNING)
            break;

        // The service is stopping
        g_ServiceStatus.dwControlsAccepted = 0;
        g_ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
        g_ServiceStatus.dwWin32ExitCode = NO_ERROR;
        g_ServiceStatus.dwCheckPoint = 4;

        if (!SetServiceStatus(g_ServiceStatusHandle, &g_ServiceStatus))
        {
            // Error handling
        }

        // Signal the worker thread to stop
        SetEvent(g_ServiceStopEvent);

        // Wait for the worker thread to stop
        WaitForSingleObject(g_ServiceStopEvent, INFINITE);

        // The service is now stopped
        g_ServiceStatus.dwControlsAccepted = 0;
        g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
        g_ServiceStatus.dwWin32ExitCode = NO_ERROR;
        g_ServiceStatus.dwCheckPoint = 5;

        if (!SetServiceStatus(g_ServiceStatusHandle, &g_ServiceStatus))
        {
            // Error handling
        }

        break;
    }
}

std::string ExecutePowerShellCommand(const std::wstring& command)
{
    std::wstring fullCommand = L"powershell.exe -Command \"& {" + command + L"}\"";
    FILE* pipe = _wpopen(fullCommand.c_str(), L"rt");
    if (!pipe)
    {
        return "Error executing PowerShell command.";
    }

    std::string result;
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
    {
        result += buffer;
    }
    _pclose(pipe);
    return result;
}

DWORD WINAPI ServiceWorkerThread(LPVOID lpParam)
{
    // Perform the main function of the service here
    // For this example, we'll call a PowerShell command and display its output using write-host

    std::wstring powerShellCommand = L"write-host 'Hello from WannaCry!'";

    std::string powerShellOutput = ExecutePowerShellCommand(powerShellCommand);
    MessageBoxA(nullptr, powerShellOutput.c_str(), "PowerShell Output", MB_OK | MB_ICONINFORMATION);

    // Signal the service is done
    SetEvent(g_ServiceStopEvent);

    return ERROR_SUCCESS;
}

