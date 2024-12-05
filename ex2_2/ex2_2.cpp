#include <windows.h>
#include <iostream>

void CopyFileWithWindowsAPI(const char* inputFile, const char* outputFile) {
    HANDLE hInFile = CreateFileA(inputFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    HANDLE hOutFile = CreateFileA(outputFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hInFile == INVALID_HANDLE_VALUE || hOutFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to open file. Error: " << GetLastError() << "\n";
        if (hInFile != INVALID_HANDLE_VALUE) CloseHandle(hInFile);
        if (hOutFile != INVALID_HANDLE_VALUE) CloseHandle(hOutFile);
        return;
    }

    const DWORD bufferSize = 1024 * 1024; // 1 MB
    char* buffer = new char[bufferSize];

    DWORD bytesRead, bytesWritten;
    while (ReadFile(hInFile, buffer, bufferSize, &bytesRead, NULL) && bytesRead > 0) {
        WriteFile(hOutFile, buffer, bytesRead, &bytesWritten, NULL);
    }

    delete[] buffer;
    CloseHandle(hInFile);
    CloseHandle(hOutFile);
}

int main() {
    const char* inputFile = "largefile.bin";
    const char* outputFile = "copy_winapi.bin";

    LARGE_INTEGER start, end, freq;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&start);

    CopyFileWithWindowsAPI(inputFile, outputFile);

    QueryPerformanceCounter(&end);
    double duration = double(end.QuadPart - start.QuadPart) / freq.QuadPart;

    std::cout << "Windows API Time: " << duration << " seconds\n";

    return 0;
}
