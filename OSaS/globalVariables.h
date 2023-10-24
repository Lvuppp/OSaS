#pragma once
#include "framework.h"

#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;													// текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];										// Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];								// имя класса главного окна
HHOOK hKeyboardHook;												// хук для отловления нажатия горячих клавиш
HWND hMainWindow;
HANDLE сompileThread;
HANDLE graphicThread;
HANDLE compileProcess;
PROCESS_INFORMATION processInfo;

std::vector<char> content;

HWND tabControlWidget;
HWND editWidget;
HWND resourcesGraphicWidget;
HWND consoleWidget;
HWND closeButton = nullptr;
HWND stopButton;
HWND treeView;
HWND compileButton;
HWND hwndBuildSettings;
HWND qmakePath;
HWND mingwMakePath;
HANDLE* filePointers;
int pageCount = 10;
int lastPage = 0;
bool isProcessCreated = false;

int modifiedHeight;
int modifiedWidth;
int editWidgetCursorIndex;
int tabIndex;
int graphicScale = 1;

std::wstring currentDirectory; 
std::wstring currentProjectPath;
std::wstring currentProFilePath;
std::wstring currentProjectName;

std::vector<std::wstring> fileNames;									// название открытых файлов
std::unordered_map<std::wstring, std::wstring> fileData;				// содержимое файлов файлов
std::unordered_map<std::wstring, bool> fileStatus;				// содержимое файлов файлов
std::unordered_map<std::wstring, std::wstring> filePathes;			// пути уже существующих фалов
