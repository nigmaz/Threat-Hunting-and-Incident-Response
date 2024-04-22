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
        L"C:\\Windows\\System32\\calc.exe", // Ðu?ng d?n d?n calc.exe
        nullptr,                             // Dòng l?nh tùy ch?n (không có)
        nullptr,                             // Thành ph?n b?o m?t quy?n (m?c d?nh)
        nullptr,                             // Thành ph?n b?o m?t quy?n (m?c d?nh)
        FALSE,                               // Không k? th?a x? lý
        0,                                   // Không t?o c?a s? c?a s?
        nullptr,                             // Không có môi tru?ng du?c ch? d?nh
        nullptr,                             // Thu m?c hi?n t?i c?a quá trình
        &si,                                 // Thông tin kh?i ch?y
        &pi                                  // Thông tin quá trình
    )) {
        printf("CreateProcess failed (%d).\n", GetLastError());
        return 1;
    }
    
    // Ðóng handle không c?n thi?t
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    
    // Ð?i m?t kho?ng th?i gian
    Sleep(5000); // Ð?i 5 giây
    
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
    
    // Ðóng handle lu?ng
    CloseHandle(hThread);
    
    return 0;
}

