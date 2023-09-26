#pragma once
#include "framework.h"

#define MAX_LOADSTRING 100

// ���������� ����������:
HINSTANCE hInst;													// ������� ���������
WCHAR szTitle[MAX_LOADSTRING];										// ����� ������ ���������
WCHAR szWindowClass[MAX_LOADSTRING];								// ��� ������ �������� ����
HHOOK hKeyboardHook;												// ��� ��� ���������� ������� ������� ������

std::vector<char> content;

HWND tabControlWidget;
HWND editWidget;
HWND closeButton = nullptr;
int tabIndex;

int modifiedHeight;
int modifiedWidth;

std::vector<std::wstring> fileNames;									// �������� �������� ������
std::unordered_map<std::wstring, std::wstring> fileData;				// ���������� ������ ������
std::unordered_map<std::wstring, std::wstring> filePathes;			// ���� ��� ������������ �����

