#include "fileHandlers.h"

#include "Resource.h"
#include "framework.h"


bool OpenFile(LPCSTR path, std::vector<char>& content)
{
    HANDLE hFile = CreateFileA(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile != INVALID_HANDLE_VALUE) {
        DWORD fileSize = GetFileSize(hFile, NULL);

        if (fileSize == INVALID_FILE_SIZE && GetLastError() != NO_ERROR) {
            CloseHandle(hFile);
            return false;
        }

        content.resize(fileSize + 1);
        DWORD bytesRead;

        if (ReadFile(hFile, content.data(), fileSize, &bytesRead, NULL)) {
            content[bytesRead] = '\0';
        }
        else {
            return false;
        }

        CloseHandle(hFile);
    }
    else {
        return false;
    }
}


bool SaveFile(LPWSTR path, LPCSTR data, int length)
{
    HANDLE file = CreateFileW(path , GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
   
    DWORD bytes;
    WriteFile(file, data, length, &bytes, NULL);

    CloseHandle(file);
    delete[] data;
    return true;
}
