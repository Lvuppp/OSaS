#pragma once

#include "framework.h"
// Отправить объявления функций, включенных в этот модуль кода:

ATOM                MyRegisterClass(HINSTANCE );
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	KeyboardProc(int, WPARAM, LPARAM );
DWORD WINAPI		WatchDirectoryThread(LPVOID lpParam);

void CALLBACK FileProcessing(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped);
void WinMenuCreation(HWND);
void WinWidgetsCreation(HWND);

bool OpenFile(LPCWSTR);
bool SaveFile(LPWSTR, LPCSTR, int);

void NewFileCommand();
void OpenFileCommand();
void CloseFileCommand();
void SaveFileCommand();

void OpenFolder(HWND hWnd);

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