#pragma once

#include "framework.h"
// Отправить объявления функций, включенных в этот модуль кода:

ATOM                MyRegisterClass(HINSTANCE );
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    BuildSettingsProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	KeyboardProc(int, WPARAM, LPARAM );
LRESULT CALLBACK    ChatProc(HWND, UINT, WPARAM, LPARAM);
DWORD WINAPI		WatchDirectoryThread(LPVOID lpParam);
DWORD WINAPI		CompileProject(LPVOID lpParam);
DWORD WINAPI		SaveFileThread(LPVOID param);

void CALLBACK FileProcessing(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped);
void WinMenuCreation(HWND);
void WinWidgetsCreation(HWND);

bool OpenFile(LPCWSTR);
bool SaveFile(LPWSTR, LPCSTR, int);

void NewFileCommand();
void OpenFileCommand();
void CloseFileCommand();
void SaveFileCommand();
DWORD WINAPI CompileProjectCommand(LPVOID param);
void StopProjectCommand();

void OpenFolder(HWND hWnd);
void OpenProject(HWND hWnd); 
void ReadLogFile();
void ClearGraph(HDC hdc);
void CreateTab(LPWSTR, LPCWSTR, LPWSTR);
void SwitchTab();
void ReplaceCloseButton(int);

void HighLightKeyWords();

void OpenColorDialog();
void ChangeFont();
void ChangeFontColor();
void PopulateTreeViewWithFiles(HWND , HTREEITEM , const std::wstring& , const int& );
void CALLBACK WriteCompletionRoutine(DWORD , DWORD , LPOVERLAPPED );

std::wstring GetPathRelativeToRoot(HWND , HTREEITEM );

DWORD WINAPI UpdateAndDraw(LPVOID lpParam);
void DrawGraph(HDC hdc, int memoryUsage);
void DrawAxes(HDC hdc, int width, int height, int maxX, int maxY, int axesMaxValue);

struct SaveFileParams {
    LPWSTR filePath;
    LPCSTR fileContent;
    int fileSize;
};

std::wstring GetHKeyPath(const wchar_t* valueName);
void SetHKeyPath(const wchar_t* valueName, const wchar_t* valueData);
void SetQMakePathCommand();
void SetMingwMakePathCommand();

void OpenBuildSettingsCommand(HWND);
void OpenChatCommand(HWND);

enum class Operation
{
    SendFile,
    UpdateReceviers,
    CloseHandle,
};


struct Package {
    Operation operation;
    int data = -1;
    int receiverId = -1;
};

void ProcessChatThread();