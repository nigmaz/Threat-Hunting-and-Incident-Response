#include <windows.h>
#include <stdio.h>

int main() {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    
    // Start the child process.
    if (!CreateProcess(
        L"C:\\Windows\\System32\\calc.exe", // �u?ng d?n d?n calc.exe
        nullptr,                             // D�ng l?nh t�y ch?n (kh�ng c�)
        nullptr,                             // Th�nh ph?n b?o m?t quy?n (m?c d?nh)
        nullptr,                             // Th�nh ph?n b?o m?t quy?n (m?c d?nh)
        FALSE,                               // Kh�ng k? th?a x? l�
        0,                                   // Kh�ng t?o c?a s? c?a s?
        nullptr,                             // Kh�ng c� m�i tru?ng du?c ch? d?nh
        nullptr,                             // Thu m?c hi?n t?i c?a qu� tr�nh
        &si,                                 // Th�ng tin kh?i ch?y
        &pi                                  // Th�ng tin qu� tr�nh
    )) {
        printf("CreateProcess failed (%d).\n", GetLastError());
        return 1;
    }
    
    // ��ng handle kh�ng c?n thi?t
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    
    // �?i m?t kho?ng th?i gian
    Sleep(5000); // �?i 5 gi�y
    
    // T?o m?t lu?ng m?i d? gi? cho calc.exe ti?p t?c ch?y
    HANDLE hThread = CreateRemoteThread(
        pi.hProcess,
        nullptr,
        0,
        (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(L"Kernel32"), "Sleep"),
        nullptr,
        0,
        nullptr
    );
    
    // ��ng handle lu?ng
    CloseHandle(hThread);
    
    return 0;
}

