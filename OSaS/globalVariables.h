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
HANDLE chatThread;
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
HWND hwndChatWindow;
HWND hChatsListBox;
HWND hChatEditWidget;
HWND qmakePath;
HWND mingwMakePath;
SOCKET clientSocket = 0;
HANDLE* filePointers;
int pageCount = 10;
int lastPage = 0;
bool isProcessCreated = false;
int id = -1;

int modifiedHeight;
int modifiedWidth;
int editWidgetCursorIndex;
int tabIndex;
int graphicScale = 1;

std::wstring currentDirectory; 
std::wstring currentProjectPath;
std::wstring currentProFilePath;
std::wstring currentProjectName;
HANDLE g_mutex = NULL;
CRITICAL_SECTION cs;

std::vector<std::wstring> fileNames;									// название открытых файлов
std::unordered_map<std::wstring, std::wstring> fileData;				// содержимое файлов файлов
std::unordered_map<std::wstring, bool> fileStatus;				// содержимое файлов файлов
std::unordered_map<std::wstring, std::wstring> filePathes;			// пути уже существующих фалов

std::wstring makeStrW = L" qmake && make | tee output.txt";
std::wstring params = L" -spec win32-g++ \"CONFIG+=debug\" \"CONFIG+=qml_debug\" && ";

sockaddr_in serverAddr;

WSADATA wsaData;