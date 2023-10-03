#pragma once
#include "framework.h"

#define MAX_LOADSTRING 100

// ���������� ����������:
HINSTANCE hInst;													// ������� ���������
WCHAR szTitle[MAX_LOADSTRING];										// ����� ������ ���������
WCHAR szWindowClass[MAX_LOADSTRING];								// ��� ������ �������� ����
HHOOK hKeyboardHook;												// ��� ��� ���������� ������� ������� ������
HWND hMainWindow;

std::vector<char> content;

HWND tabControlWidget;
HWND editWidget;
HWND closeButton = nullptr;
HWND treeView;
HANDLE* filePointers;
int pageCount = 10;
int lastPage = 0;

int modifiedHeight;
int modifiedWidth;
int editWidgetCursorIndex;
int tabIndex;

std::wstring currentDirectory; 

std::vector<std::wstring> fileNames;									// �������� �������� ������
std::unordered_map<std::wstring, std::wstring> fileData;				// ���������� ������ ������
std::unordered_map<std::wstring, bool> fileStatus;				// ���������� ������ ������
std::unordered_map<std::wstring, std::wstring> filePathes;			// ���� ��� ������������ �����

