// header.h: включаемый файл для стандартных системных включаемых файлов
// или включаемые файлы для конкретного проекта
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Исключите редко используемые компоненты из заголовков Windows
#define _WIN32_IE 0x500
// Файлы заголовков Windows
#include <windows.h>
#include <Shlwapi.h> 
// Файлы заголовков среды выполнения C
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <commdlg.h> 
#include <commctrl.h> 
#include <CommCtrl.h>

#include <string>
#include<vector>
#include<string>
#include<unordered_map>
#include<fstream>
#include <iostream>
#include <codecvt>
#include <Richedit.h>
#include <Shlobj.h>
#include <ShellAPI.h>