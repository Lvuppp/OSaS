#pragma once
#include "framework.h"

bool OpenFile(LPCSTR path, std::vector<char>& content);

bool SaveFile(LPWSTR path, LPCSTR data, int length);