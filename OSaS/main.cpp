// OSaS.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "prototypes.h"
#include "tabHandlers.h"
#include "fileHandlers.h"

#include "commandsPrototypes.h"
#include "globalVariables.h"
#include "main.h"

int WINAPI wWinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPWSTR    lpCmdLine,
    int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);



    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_OSAS, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_OSAS));
    hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, hInstance, 0);

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    std::vector<char> v;
    OpenFile("sdfsdfds", v);
    UnhookWindowsHookEx(hKeyboardHook);

    return (int)msg.wParam;
}


//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_OSAS));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_OSAS);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case ENM_UPDATE:
            HighLightKeyWords();
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        case NEW_FILE_COMMAND:
            NewFileCommand();
            break;
        case OPEN_FILE_COMMAND:
            OpenFileCommand();
            break;
        case SAVE_FILE_COMMAND:
            SaveFileCommand();
            break;
        case CLOSE_FILE_COMMAND:
            CloseFileCommand();
            break;
        case CLOSE_TAB_COMMAND:
            CloseFileCommand();
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_NOTIFY:
    {
        NMHDR* nmhdr = (NMHDR*)lParam;
        if (nmhdr->code == TCN_SELCHANGE) {
            SwitchTab();
        }
    }
    break;
    case WM_SIZE:

        modifiedWidth = LOWORD(lParam);
        modifiedHeight = HIWORD(lParam);

        MoveWindow(tabControlWidget, 1, 1, modifiedWidth, 30, TRUE);
        MoveWindow(editWidget, 1, 31, modifiedWidth, modifiedHeight, TRUE);

        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_CREATE:
        WinWidgetsCreation(hWnd);
        WinMenuCreation(hWnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0 && wParam == WM_KEYDOWN)
    {
        KBDLLHOOKSTRUCT* pKeyInfo = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);

        if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
        {
            if (GetAsyncKeyState(VK_SHIFT) & 0x7000) {

            }
            switch (pKeyInfo->vkCode)
            {
            case 'S':
                SaveFileCommand();
                break;
            case 'O':
                OpenFileCommand();
                break;
            case 'C':
                CloseFileCommand();
                break;
            case 'N':
                NewFileCommand();
                break;
            default:
                break;
            }
        }

    }

    return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
}


void WinWidgetsCreation(HWND hWnd) {
    tabControlWidget = CreateWindowEx(0, WC_TABCONTROL, L"", WS_CHILD | WS_VISIBLE | TCS_FIXEDWIDTH,
        1, 0, 500, 30, hWnd, (HMENU)IDC_TABCTRL, GetModuleHandle(NULL), NULL);
    LoadLibrary(TEXT("Msftedit.dll"));

    editWidget = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        MSFTEDIT_CLASS, L"",
        WS_BORDER | WS_CHILD | WS_VISIBLE | ES_MULTILINE | WS_VSCROLL | WS_TABSTOP,
        0, 0, 800, 600,
        hWnd, NULL, hInst, NULL
    );
    UpdateWindow(editWidget);
}


void WinMenuCreation(HWND hWnd) {
    HMENU fileMenu = CreateMenu();
    HMENU fileSettings = CreateMenu();
    HMENU fileSubMenu = CreateMenu();

    AppendMenu(fileSubMenu, MF_STRING, NEW_FILE_COMMAND, L"New");
    AppendMenu(fileSubMenu, MF_STRING, OPEN_FILE_COMMAND, L"Open");
    AppendMenu(fileSubMenu, MF_STRING, SAVE_FILE_COMMAND, L"Save");
    AppendMenu(fileSubMenu, MF_STRING, CLOSE_FILE_COMMAND, L"Close");

    AppendMenu(fileMenu, MF_POPUP, (UINT_PTR)fileSubMenu, L"File");
    AppendMenu(fileMenu, MF_STRING, NULL, L"Settings");

    SetMenu(hWnd, fileMenu);
}

void CloseFileCommand()
{
    if (TabCtrl_GetItemCount(tabControlWidget) == 0)
    {
        return;
    }

    int result = MessageBox(NULL, L"Do you want to save the file?", L"Save File", MB_YESNOCANCEL | MB_ICONQUESTION);

    if (result == IDYES)
    {
        SaveFileCommand();
    }

    auto name = fileNames[tabIndex];

    fileNames.erase(fileNames.begin() + tabIndex);
    filePathes.erase(name);
    fileData.erase(name);

    TabCtrl_DeleteItem(tabControlWidget, tabIndex);

    SwitchTab();
}

void SaveFileCommand()
{
    if (!TabCtrl_GetItemCount(tabControlWidget))
    {
        return;
    }
    tabIndex = TabCtrl_GetCurSel(tabControlWidget);
    int length = GetWindowTextLength(editWidget) + 1;
    LPSTR data = new CHAR[length];

    length = GetWindowTextA(editWidget, data, length);
    auto it = filePathes.find(fileNames[tabIndex]);

    if (it != filePathes.end())
    {
        LPWSTR path = const_cast<LPWSTR>((*it).second.c_str());
        SaveFile(path, data, length);
    }
    else
    {
        OPENFILENAME ofn;
        wchar_t szFileName[1024] = L"";

        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = NULL;
        ofn.lpstrFilter = L"All Files (*.*)\0*.*\0";
        ofn.lpstrFile = szFileName;
        ofn.nMaxFile = MAX_PATH;
        ofn.Flags = OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

        if (GetOpenFileName(&ofn) == TRUE)
        {
            SaveFile(szFileName, data, length);

            std::wstring name = szFileName;
            name = name.substr(name.find_last_of('\\') + 1);

            fileData[name] = fileData[fileNames[tabIndex]];
            fileNames[tabIndex] = name;
            filePathes[name] = szFileName;

            TCITEM tcItem = { 0 };
            tcItem.mask = TCIF_TEXT;
            tcItem.pszText = const_cast<LPWSTR>(name.data());

            TabCtrl_SetItem(tabControlWidget, tabIndex, &tcItem);
        }
    }
}
void NewFileCommand()
{
    int num = 1;
    auto it = fileData.find(L"File" + std::to_wstring(num));
    while (it != fileData.end())
    {
        num++;
        it = fileData.find(L"File" + std::to_wstring(num));
    }
    std::wstring fileName = L"File" + std::to_wstring(num);
    CreateTab(const_cast<LPWSTR>(fileName.c_str()), L"", nullptr);
}



void OpenFileCommand()
{
    OPENFILENAME ofn;
    wchar_t szFileName[1024] = L"";
    std::vector<char> pathBuffer(1024);

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = L"Text Files\0*.cpp;*.h\0";;
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn) == TRUE)
    {

        std::vector<char> content;
        WideCharToMultiByte(CP_UTF8, 0, szFileName, -1, pathBuffer.data(), sizeof(szFileName), NULL, NULL);
        OpenFile(pathBuffer.data(), content);

        std::wstring name = szFileName;
        name = name.substr(name.find_last_of('\\') + 1);
        std::wstring data = std::wstring(content.begin(), content.end());

        LPWSTR fileName = const_cast<LPWSTR>(name.data());
        LPCWSTR fileData = data.c_str();
        LPWSTR filePath = szFileName;

        CreateTab(fileName, fileData, filePath);

    }

}




void ReplaceCloseButton(int currentTabIndex)
{
   /* RECT tabRect;
    TabCtrl_GetItemRect(tabControlWidget, currentTabIndex, &tabRect);

    if (closeButton)
    {
        MoveWindow(closeButton, tabRect.right - 20, tabRect.top + 2, 15, 15, TRUE);
    }
    else
    {
        closeButton = CreateWindowA("button", "X", WS_CHILD | WS_VISIBLE,
            tabRect.right - 20, tabRect.top + 2, 15, 15, tabControlWidget,
            (HMENU)CLOSE_TAB_COMMAND, NULL, NULL);
    }*/

}

void HighLightKeyWords()
{
    FINDTEXTEXW findText;
    findText.chrg.cpMin = 0; 
    findText.chrg.cpMax = -1; 
    const wchar_t* wordToFind = L"class";
    findText.lpstrText = wordToFind;

    int wordLength = wcslen(wordToFind);

    while (SendMessage(editWidget, EM_FINDTEXTEX, FR_DOWN | FR_MATCHCASE, reinterpret_cast<LPARAM>(&findText)) != -1) {
        CHARFORMAT2 charFormat;
        charFormat.cbSize = sizeof(CHARFORMAT2);
        charFormat.dwMask = CFM_COLOR | CFM_UNDERLINE;
        charFormat.crTextColor = RGB(0, 255, 0);
        charFormat.dwEffects = CFE_UNDERLINE;

        SendMessage(editWidget, EM_SETCHARFORMAT, SCF_SELECTION, reinterpret_cast<LPARAM>(&charFormat));

        findText.chrg.cpMin = findText.chrgText.cpMax;
        findText.chrg.cpMax = -1;
    }
}


void SwitchTab()
{
    if (TabCtrl_GetItemCount(tabControlWidget) == 0)
    {
        DestroyWindow(closeButton);
        closeButton = nullptr;
        SetWindowTextW(editWidget, L"");
        return;
    }

    int length = GetWindowTextLengthW(editWidget) + 1;
    LPWSTR data = new WCHAR[length];
    length = GetWindowTextW(editWidget, data, length);
    fileData[fileNames[tabIndex]] = data;

    tabIndex = TabCtrl_GetCurSel(tabControlWidget);

    SendMessage(editWidget, EM_SETSEL, 0, -1);
    SendMessage(editWidget, EM_REPLACESEL, TRUE, 
        reinterpret_cast<LPARAM>(fileData[fileNames[tabIndex]].c_str()));

    ReplaceCloseButton(tabIndex);
}


void CreateTab(LPWSTR name, LPCWSTR data, LPWSTR path)
{
    if (TabCtrl_GetItemCount(tabControlWidget) != 0)
    {
        int length = GetWindowTextLengthW(editWidget) + 1;
        LPWSTR newData = new WCHAR[length];
        length = GetWindowTextW(editWidget, newData, length);
        fileData[fileNames[tabIndex]] = newData;
    }


    TCITEM tie;
    tie.mask = TCIF_TEXT;
    tie.pszText = name;
    TabCtrl_InsertItem(tabControlWidget, fileNames.size(), &tie);

    fileNames.emplace_back(name);
    fileData.insert(std::make_pair(fileNames.back(), std::wstring(data)));

    if (path)
    {
        filePathes.insert(std::make_pair(fileNames.back(), std::wstring(path)));
    }

    tabIndex = fileNames.size() - 1;
    
    auto lData = reinterpret_cast<LPARAM>(data);

    SendMessage(editWidget, EM_SETSEL, 0, -1);
    SendMessage(editWidget, EM_REPLACESEL, TRUE, lData);

    ReplaceCloseButton(fileNames.size() - 1);
    TabCtrl_SetCurSel(tabControlWidget, fileNames.size() - 1);
}
