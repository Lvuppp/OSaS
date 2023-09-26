#pragma once
#include "framework.h"

#define MAX_LOADSTRING 100

// √лобальные переменные:
HINSTANCE hInst;													// текущий экземпл€р
WCHAR szTitle[MAX_LOADSTRING];										// “екст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];								// им€ класса главного окна
HHOOK hKeyboardHook;												// хук дл€ отловлени€ нажати€ гор€чих клавиш

std::vector<char> content;

HWND tabControlWidget;
HWND editWidget;
HWND closeButton = nullptr;
int tabIndex;

int modifiedHeight;
int modifiedWidth;

std::vector<std::wstring> fileNames;									// название открытых файлов
std::unordered_map<std::wstring, std::wstring> fileData;				// содержимое файлов файлов
std::unordered_map<std::wstring, std::wstring> filePathes;			// пути уже существующих фалов

