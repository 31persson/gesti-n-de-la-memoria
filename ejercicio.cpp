#include <windows.h>
#include <tchar.h>
#include <stdio.h>

#define SIZE 4096

int _tmain() {
    HANDLE hMapFile;
    LPCTSTR pBuf;

    hMapFile = CreateFileMapping(
            INVALID_HANDLE_VALUE,
            NULL,
            PAGE_READWRITE,
            0,
            SIZE,
            _T("SharedMemory"));

    if (hMapFile == NULL) {
        _tprintf(_T("Could not create file mapping object (%d).\n"), GetLastError());
        return 1;
    }

    pBuf = (LPTSTR)MapViewOfFile(
            hMapFile,
            FILE_MAP_ALL_ACCESS,
            0,
            0,
            SIZE);

    if (pBuf == NULL) {
        _tprintf(_T("Could not map view of file (%d).\n"), GetLastError());
        CloseHandle(hMapFile);
        return 1;
    }

    PROCESS_INFORMATION piProcInfo;
    STARTUPINFO siStartInfo;

    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));

    siStartInfo.cb = sizeof(STARTUPINFO);

    // Create the child process.
    if (!CreateProcess(NULL,
                       _T("child_process.exe"),  // Nombre del ejecutable del proceso hijo
                       NULL,
                       NULL,
                       FALSE,
                       0,
                       NULL,
                       NULL,
                       &siStartInfo,
                       &piProcInfo)) {
        fprintf(stderr, "CreateProcess failed (%d).\n", GetLastError());
        return 1;
    }

    _tcscpy((LPTSTR)pBuf, _T("Hello, child process!"));

    WaitForSingleObject(piProcInfo.hProcess, INFINITE);

    UnmapViewOfFile(pBuf);
    CloseHandle(hMapFile);

    CloseHandle(piProcInfo.hProcess);
    CloseHandle(piProcInfo.hThread);

    return 0;
}
