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

std::vector<std::wstring> fileNames;									// �������� �������� ������
std::unordered_map<std::wstring, std::wstring> fileData;				// ���������� ������ ������
std::unordered_map<std::wstring, bool> fileStatus;				// ���������� ������ ������
std::unordered_map<std::wstring, std::wstring> filePathes;			// ���� ��� ������������ �����
