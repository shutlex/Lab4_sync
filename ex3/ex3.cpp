#include <windows.h>
#include <stdio.h>

#define MAX_FILES 3
#define BUFFER_SIZE 1024

typedef struct {
    HANDLE hFile;
    OVERLAPPED overlapped;
    char buffer[BUFFER_SIZE];
    DWORD bytesRead;
    DWORD bytesWritten;
    HANDLE hEvent;  
} FileOperation;

void CreateTestFile(LPCWSTR filename) {
    
    HANDLE hFile = CreateFile(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_OVERLAPPED, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        wprintf(L"Не вдалося створити файл: %s\n", filename);
        return;
    }

    
    const char* testData = "Тестові дані для файлу.\n";
    DWORD bytesWritten;
    WriteFile(hFile, testData, strlen(testData), &bytesWritten, NULL);

    wprintf(L"Тестові дані записано в файл: %s\n", filename);
    CloseHandle(hFile);
}

void ProcessFileAsync(FileOperation* fileOp, LPCWSTR filename) {
    wprintf(L"Відкриваємо файл: %s\n", filename); 

    fileOp->hFile = CreateFile(filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
    if (fileOp->hFile == INVALID_HANDLE_VALUE) {
        wprintf(L"Не вдалося відкрити файл: %s\n", filename);
        return;
    }

    
    ZeroMemory(&fileOp->overlapped, sizeof(OVERLAPPED));
    fileOp->overlapped.Offset = 0;

    
    fileOp->hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (fileOp->hEvent == NULL) {
        wprintf(L"Не вдалося створити подію: %s\n", filename);
        CloseHandle(fileOp->hFile);
        return;
    }

    
    fileOp->overlapped.hEvent = fileOp->hEvent;

    wprintf(L"Читання файлу...\n"); // Додано повідомлення для відлагодження
    if (!ReadFile(fileOp->hFile, fileOp->buffer, BUFFER_SIZE, &fileOp->bytesRead, &fileOp->overlapped)) {
        if (GetLastError() != ERROR_IO_PENDING) {
            wprintf(L"Помилка при читанні файлу: %s\n", filename);
            CloseHandle(fileOp->hFile);
            CloseHandle(fileOp->hEvent);
            return;
        }
    }

    
    DWORD dwWaitResult = WaitForSingleObject(fileOp->hEvent, INFINITE);
    if (dwWaitResult == WAIT_FAILED) {
        wprintf(L"Помилка при очікуванні завершення операції: %s\n", filename);
        CloseHandle(fileOp->hFile);
        CloseHandle(fileOp->hEvent);
        return;
    }

    if (!GetOverlappedResult(fileOp->hFile, &fileOp->overlapped, &fileOp->bytesRead, TRUE)) {
        wprintf(L"Помилка при отриманні результату зчитування файлу: %s\n", filename);
    }

    wprintf(L"Прочитано %d байт з файлу %s\n", fileOp->bytesRead, filename);

    wprintf(L"Запис у файл...\n"); 
    if (!WriteFile(fileOp->hFile, fileOp->buffer, fileOp->bytesRead, &fileOp->bytesWritten, &fileOp->overlapped)) {
        if (GetLastError() != ERROR_IO_PENDING) {
            wprintf(L"Помилка при записі файлу: %s\n", filename);
            CloseHandle(fileOp->hFile);
            CloseHandle(fileOp->hEvent);
            return;
        }
    }

    
    dwWaitResult = WaitForSingleObject(fileOp->hEvent, INFINITE);
    if (dwWaitResult == WAIT_FAILED) {
        wprintf(L"Помилка при очікуванні завершення операції запису: %s\n", filename);
        CloseHandle(fileOp->hFile);
        CloseHandle(fileOp->hEvent);
        return;
    }

    if (!GetOverlappedResult(fileOp->hFile, &fileOp->overlapped, &fileOp->bytesWritten, TRUE)) {
        wprintf(L"Помилка при отриманні результату запису файлу: %s\n", filename);
    }

    wprintf(L"Записано %d байт у файл %s\n", fileOp->bytesWritten, filename);

    CloseHandle(fileOp->hFile);
    CloseHandle(fileOp->hEvent);
}

int main() {
    FileOperation fileOps[MAX_FILES];

    
    LPCWSTR filenames[MAX_FILES] = { L"file1.txt", L"file2.txt", L"file3.txt" };

    
    for (int i = 0; i < MAX_FILES; i++) {
        CreateTestFile(filenames[i]);
    }

    
    for (int i = 0; i < MAX_FILES; i++) {
        ProcessFileAsync(&fileOps[i], filenames[i]);
    }

    return 0;
}
