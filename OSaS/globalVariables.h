#pragma once
#include "framework.h"

#define MAX_LOADSTRING 100

// ���������� ����������:
HINSTANCE hInst;													// ������� ���������
WCHAR szTitle[MAX_LOADSTRING];										// ����� ������ ���������
WCHAR szWindowClass[MAX_LOADSTRING];								// ��� ������ �������� ����
HHOOK hKeyboardHook;												// ��� ��� ���������� ������� ������� ������
HWND hMainWindow;
HANDLE �ompileThread;
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

std::vector<std::wstring> fileNames;									// �������� �������� ������
std::unordered_map<std::wstring, std::wstring> fileData;				// ���������� ������ ������
std::unordered_map<std::wstring, bool> fileStatus;				// ���������� ������ ������
std::unordered_map<std::wstring, std::wstring> filePathes;			// ���� ��� ������������ �����

std::wstring makeStrW = L" qmake && make | tee output.txt";
std::wstring params = L" -spec win32-g++ \"CONFIG+=debug\" \"CONFIG+=qml_debug\" && ";

sockaddr_in serverAddr;

WSADATA wsaData;