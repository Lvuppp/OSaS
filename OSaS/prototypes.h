#pragma once

#include "framework.h"
// Отправить объявления функций, включенных в этот модуль кода:

ATOM                MyRegisterClass(HINSTANCE );
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	KeyboardProc(int, WPARAM, LPARAM );

void WinMenuCreation(HWND);
void WinWidgetsCreation(HWND);


bool OpenFile(LPCSTR, std::vector<char>&);
bool SaveFile(LPWSTR, LPCSTR, int);


void NewFileCommand();
void OpenFileCommand();
void CloseFileCommand();
void SaveFileCommand();


void CreateTab(LPWSTR, LPCWSTR, LPWSTR);
void SwitchTab();
void ReplaceCloseButton(int);

void HighLightKeyWords();

void OpenColorDialog();
void ChangeFont();
void ChangeFontColor();