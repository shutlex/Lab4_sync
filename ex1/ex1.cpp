#include <windows.h>
#include <tchar.h>
#include <iostream>
#include <aclapi.h>

void PrintFileTime(const FILETIME& fileTime, const char* label) {
    SYSTEMTIME systemTime;
    FileTimeToSystemTime(&fileTime, &systemTime);

    std::cout << label << ": "
        << systemTime.wDay << "/"
        << systemTime.wMonth << "/"
        << systemTime.wYear << " "
        << systemTime.wHour << ":"
        << systemTime.wMinute << ":"
        << systemTime.wSecond << "\n";
}

void GetFileOwner(const char* fileName) {
    PSID pSidOwner = NULL;
    PSECURITY_DESCRIPTOR pSD = NULL;

    if (GetNamedSecurityInfoA(fileName, SE_FILE_OBJECT, OWNER_SECURITY_INFORMATION, &pSidOwner, NULL, NULL, NULL, &pSD) == ERROR_SUCCESS) {
        char ownerName[256];
        char domainName[256];
        DWORD ownerNameSize = sizeof(ownerName);
        DWORD domainNameSize = sizeof(domainName);
        SID_NAME_USE sidType;

        if (LookupAccountSidA(NULL, pSidOwner, ownerName, &ownerNameSize, domainName, &domainNameSize, &sidType)) {
            std::cout << "Owner: " << domainName << "\\" << ownerName << "\n";
        }
        else {
            std::cerr << "Failed to lookup account SID.\n";
        }
    }
    else {
        std::cerr << "Failed to retrieve security information.\n";
    }

    if (pSD) LocalFree(pSD);
}

void GetFileAttributesInfo(const char* filePath) {
    DWORD attributes = GetFileAttributesA(filePath);
    if (attributes == INVALID_FILE_ATTRIBUTES) {
        std::cerr << "Failed to get file attributes. Error: " << GetLastError() << "\n";
        return;
    }

    std::cout << "Attributes:\n";
    if (attributes & FILE_ATTRIBUTE_READONLY) std::cout << "- Readonly\n";
    if (attributes & FILE_ATTRIBUTE_HIDDEN) std::cout << "- Hidden\n";
    if (attributes & FILE_ATTRIBUTE_SYSTEM) std::cout << "- System\n";
    if (attributes & FILE_ATTRIBUTE_ARCHIVE) std::cout << "- Archive\n";
    if (attributes & FILE_ATTRIBUTE_DIRECTORY) std::cout << "- Directory\n";
    if (attributes & FILE_ATTRIBUTE_TEMPORARY) std::cout << "- Temporary\n";

    HANDLE hFile = CreateFileA(filePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to open file. Error: " << GetLastError() << "\n";
        return;
    }

    LARGE_INTEGER fileSize;
    if (GetFileSizeEx(hFile, &fileSize)) {
        std::cout << "File size: " << fileSize.QuadPart << " bytes\n";
    }
    else {
        std::cerr << "Failed to get file size. Error: " << GetLastError() << "\n";
    }

    FILETIME creationTime, lastAccessTime, lastWriteTime;
    if (GetFileTime(hFile, &creationTime, &lastAccessTime, &lastWriteTime)) {
        PrintFileTime(creationTime, "Creation time");
        PrintFileTime(lastAccessTime, "Last access time");
        PrintFileTime(lastWriteTime, "Last write time");
    }
    else {
        std::cerr << "Failed to get file times. Error: " << GetLastError() << "\n";
    }

    CloseHandle(hFile);

    GetFileOwner(filePath);
}

int main() {
    const char* filePath = "C:\\Users\\prokh\\source\\repos\\Lab4_sysc\\ex1_prokhorenko.txt";




    std::cout << "File: " << filePath << "\n";
    GetFileAttributesInfo(filePath);

    return 0;
}
