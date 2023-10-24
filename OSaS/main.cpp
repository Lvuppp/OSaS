// OSaS.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "prototypes.h"
#include "fileHandlers.h"

#include "globalVariables.h"
#include "main.h"
#include <assert.h>

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
    UnhookWindowsHookEx(hKeyboardHook);

    return (int)msg.wParam;
}
void MonitorDirectoryChanges() {
    HANDLE directoryHandle = CreateFileW(
        currentDirectory.c_str(),
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
        NULL
    );

    if (directoryHandle == INVALID_HANDLE_VALUE) {
        OutputDebugString(L"Failed to open directory: " + GetLastError());
        return;
    }

    DWORD bufferSize = 1024;
    BYTE buffer[1024];
    DWORD bytesReturned;
    OVERLAPPED overlapped = { 0 };

    while (true) {
        ZeroMemory(buffer, bufferSize);

        BOOL result = ReadDirectoryChangesW(
            directoryHandle,
            &buffer,
            bufferSize,
            TRUE,
            FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME,
            &bytesReturned,
            &overlapped,
            NULL
        );

        if (!result) {
            OutputDebugString(L"ReadDirectoryChangesW failed: " + GetLastError());
            break;
        }

        // Wait for the completion of the overlapped operation
        DWORD waitResult = WaitForSingleObject(directoryHandle, INFINITE);

        if (waitResult == WAIT_OBJECT_0) {
            OutputDebugString(L"Directory changed.");
            // TODO: Handle the directory change as needed
        }
        else {
            OutputDebugString(L"WaitForSingleObject failed: " + GetLastError());
            break;
        }
    }

    CloseHandle(directoryHandle);
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

    WNDCLASSEXW wcBuildSettingsWindow{ sizeof(WNDCLASSEX) };

    wcBuildSettingsWindow.lpfnWndProc = BuildSettingsProc;
    wcBuildSettingsWindow.hInstance = hInstance;
    wcBuildSettingsWindow.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(LTGRAY_BRUSH));
    wcBuildSettingsWindow.lpszClassName = L"BuildSettingsWindowClass";
    RegisterClassExW(&wcex);
    return RegisterClassExW(&wcBuildSettingsWindow);
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

    hMainWindow = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hMainWindow)
    {
        return FALSE;
    }

    ShowWindow(hMainWindow, nCmdShow);
    UpdateWindow(hMainWindow);

    if (сompileThread != NULL)
    {
        StopProjectCommand();
    }

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
        std::wstring messageNum = std::to_wstring(wmId);
       // OutputDebugString(messageNum.c_str());
        switch (wmId)
        {
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        case HIGHTLIGHT_TEXT:
            if (editWidget) {
                HighLightKeyWords();
            }
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
        case CHANGE_BG_COLOR:
            OpenColorDialog();
            break;
        case CHANGE_FONT:
            ChangeFont();
            break;
        case CHANGE_FONT_COLOR:
            ChangeFontColor();
            break;
        case OPEN_FOLDER_COMMAND:
            OpenFolder(hWnd);
            break;
        case OPEN_PROJECT_COMMAND:
            OpenProject(hWnd);
            break;
        case COMPILE_PROJECT_COMMAND:
            CompileProjectCommand();
            break;
        case STOP_PROJECT_COMMAND:
            StopProjectCommand();
            break;
        case CHANGE_BUILD_SETTINGS_COMMAND:
            OpenBuildSettingsCommand(hWnd);
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
        else if (nmhdr->idFrom == IDC_TREEVIEW && nmhdr->code == NM_DBLCLK) {
            OutputDebugString((currentDirectory + L"\\" + GetPathRelativeToRoot(treeView,
                TreeView_GetSelection((HWND)nmhdr->hwndFrom))).c_str());
            OpenFile((currentDirectory + L"\\" + GetPathRelativeToRoot(treeView,
                TreeView_GetSelection((HWND)nmhdr->hwndFrom))).c_str());
        }
    }
    break;
    case WM_SIZE:
        modifiedWidth = LOWORD(lParam);
        modifiedHeight = HIWORD(lParam);

        MoveWindow(tabControlWidget, 302, 1, modifiedWidth - 300, 30, TRUE);
        MoveWindow(editWidget, 302, 31, modifiedWidth - 300, modifiedHeight - 200, TRUE);
        MoveWindow(treeView, 1, 31, 300, modifiedHeight - 200, TRUE);
        MoveWindow(consoleWidget, 300, modifiedHeight - 170, modifiedWidth, 200, TRUE);
        MoveWindow(resourcesGraphicWidget, 1, modifiedHeight - 170, 300, 200, TRUE);
        break;
    case WM_USER + 1:
        TreeView_DeleteAllItems(treeView);
        PopulateTreeViewWithFiles(treeView,TreeView_GetRoot(treeView), currentDirectory, 0 );
        break;
    case WM_USER + 2:
        CloseHandle(сompileThread);
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
            switch (pKeyInfo->vkCode)
            {
            case 'S':
                SaveFileCommand();
                break;
            case 'O':
                OpenFileCommand();
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

    resourcesGraphicWidget = CreateWindowEx(0, L"static", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
        300, 0, 300, 200, hWnd, NULL, GetModuleHandle(NULL), NULL);

    editWidget = CreateWindowEx(0, MSFTEDIT_CLASS, L"", WS_BORDER | WS_CHILD | WS_VISIBLE |
        ES_MULTILINE | WS_VSCROLL | WS_TABSTOP, 302, 30, 400, 600, hWnd, (HMENU)IDC_RICHEDIT, hInst, NULL);

    treeView = CreateWindowEx(0, WC_TREEVIEW, L"", WS_VISIBLE | WS_CHILD | WS_BORDER | TVS_HASBUTTONS | 
        TVS_LINESATROOT | TVS_SHOWSELALWAYS | TVS_HASLINES, 1, 30, 300, 600, hWnd, (HMENU)IDC_TREEVIEW,
        hInst, NULL); 

    compileButton = CreateWindowEx(0, L"button", L"Compile", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        0, 0, 70, 30, hWnd, (HMENU)COMPILE_PROJECT_COMMAND, hInst, NULL);

    stopButton = CreateWindowEx(0, L"button", L"Stop", WS_CHILD | BS_PUSHBUTTON,
        70, 0, 70, 30, hWnd, (HMENU)STOP_PROJECT_COMMAND, hInst, NULL);

    consoleWidget = CreateWindowEx(0, L"edit", L"", WS_BORDER | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
        300, 632, 400, 200, hWnd, NULL, hInst, NULL);

    wchar_t text[] = L"Root";
    TVITEM tvItem = { 0 };
    tvItem.mask = TVIF_TEXT;
    tvItem.pszText = text;
    TVINSERTSTRUCT tvInsert = { 0 };
    tvInsert.hParent = TVI_ROOT;
    tvInsert.item = tvItem;

    MonitorDirectoryChanges();

    SendMessage(editWidget, EM_SETEVENTMASK, 0, ENM_CHANGE);
    int tabSizeInSpaces = 4;
    SendMessage(editWidget, EM_SETTABSTOPS, 1, (LPARAM)&tabSizeInSpaces);

    
}

void WinMenuCreation(HWND hWnd) {
    HMENU fileMenu = CreateMenu();
    HMENU fileSubMenu = CreateMenu();
    HMENU settingsSubMenu = CreateMenu();
    HMENU comilerSettingsSubMenu = CreateMenu();

    AppendMenu(fileSubMenu, MF_STRING, OPEN_PROJECT_COMMAND, L"Open project");
    AppendMenu(fileSubMenu, MF_STRING, NEW_FILE_COMMAND, L"New File");
    AppendMenu(fileSubMenu, MF_STRING, OPEN_FILE_COMMAND, L"Open File");
    AppendMenu(fileSubMenu, MF_STRING, SAVE_FILE_COMMAND, L"Save File");
    AppendMenu(fileSubMenu, MF_STRING, CLOSE_FILE_COMMAND, L"Close");

    AppendMenu(settingsSubMenu, MF_STRING, CHANGE_BG_COLOR, L"Bg");
    AppendMenu(settingsSubMenu, MF_STRING, CHANGE_FONT, L"Font");
    AppendMenu(settingsSubMenu, MF_STRING, CHANGE_FONT_COLOR, L"Font color");
    AppendMenu(settingsSubMenu, MF_STRING, HIGHTLIGHT_TEXT, L"Highlight text");

    AppendMenu(comilerSettingsSubMenu, MF_STRING, CHANGE_BUILD_SETTINGS_COMMAND, L"Set build params");

    AppendMenu(fileMenu, MF_POPUP, (UINT_PTR)fileSubMenu, L"File");
    AppendMenu(fileMenu, MF_POPUP, (UINT_PTR)settingsSubMenu, L"Settings");
    AppendMenu(fileMenu, MF_POPUP, (UINT_PTR)comilerSettingsSubMenu, L"Build Settings");

    SetMenu(hWnd, fileMenu);
}

void CloseFileCommand()
{
    if (TabCtrl_GetItemCount(tabControlWidget) == 0)
    {
        return;
    }

    int result = MessageBox(NULL, L"Do you want to save the file?", L"Save File", MB_YESNOCANCEL | MB_ICONQUESTION);

    if (result == IDCANCEL) {
        return;
    }
    else if (result == IDYES)
    {
        SaveFileCommand();
    }
   
    auto name = fileNames[tabIndex];

    filePathes.erase(name);
    fileData.erase(name);
    fileNames.erase(fileNames.begin() + tabIndex);
    SwitchTab();
    TabCtrl_DeleteItem(tabControlWidget, tabIndex);
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

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = L"Text Files\0*.cpp;*.h\0";
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn) == TRUE)
    {
        LPCWSTR path = szFileName;

        if (OpenFile(path)) {
            currentDirectory = path;
        }
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
    DWORD cursorPos;
    SendMessage(editWidget, EM_GETSEL, reinterpret_cast<WPARAM>(&cursorPos), NULL);

    const std::unordered_map<std::wstring, COLORREF> wordColors = {
        { L"class", RGB(0, 255, 0) },
        { L"struct", RGB(0, 255, 0) },
        { L"static", RGB(0, 0, 255) },
        { L"int", RGB(0, 0, 255) },
        { L"auto", RGB(0, 0, 255) },
        { L"float", RGB(0, 0, 255) },
        { L"void", RGB(0, 0, 255) },
        { L"string", RGB(0, 0, 255) },
        { L"double", RGB(0, 0, 255) },
        { L"static", RGB(0, 0, 255) },
        { L"while", RGB(255, 0, 255) }, 
        { L"if", RGB(255, 0, 255) } ,
        { L"switch", RGB(255, 0, 255) } ,
        { L"return", RGB(255, 0, 255) } ,
    };

    CHARFORMAT2 charFormat;
    memset(&charFormat, 0, sizeof(CHARFORMAT2));
    charFormat.cbSize = sizeof(CHARFORMAT2);
    charFormat.dwMask = CFM_COLOR;
    charFormat.crTextColor = RGB(0, 0, 0);

    int textLength = GetWindowTextLength(editWidget);
    std::wstring text(textLength + 1, L'\0');
    GetWindowText(editWidget, &text[0], textLength + 1);


    for (auto& entry : wordColors)
    {
        FINDTEXTEXW findText;
        findText.chrg.cpMin = 0;
        findText.chrg.cpMax = -1;
        findText.lpstrText = entry.first.c_str();

        while (SendMessage(editWidget, EM_FINDTEXTEX, FR_DOWN, (LPARAM)&findText) != -1)
        {
            CHARRANGE selRange;
            selRange.cpMin = findText.chrgText.cpMin;
            selRange.cpMax = findText.chrgText.cpMax;
            SendMessage(editWidget, EM_EXSETSEL, 0, (LPARAM)&selRange);

            charFormat.crTextColor = entry.second;
            SendMessage(editWidget, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&charFormat);

            findText.chrg.cpMin = findText.chrgText.cpMax;
            findText.chrg.cpMax = -1;

        }
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
    tie.mask = TCIF_TEXT | TCIS_BUTTONPRESSED;
    tie.pszText = name;
    TabCtrl_InsertItem(tabControlWidget, fileNames.size(), &tie);

    fileNames.emplace_back(name);
    fileData.insert(std::make_pair(fileNames.back(), std::wstring(data)));

    if (path)
    {
        filePathes.insert(std::make_pair(fileNames.back(), std::wstring(path)));
    }

    tabIndex = fileNames.size() - 1;
    SetWindowTextW(editWidget, data);

    ReplaceCloseButton(fileNames.size() - 1);
    TabCtrl_SetCurSel(tabControlWidget, fileNames.size() - 1);
}

void OpenColorDialog()
{
    CHOOSECOLOR cc = { sizeof(CHOOSECOLOR) };
    static COLORREF custColors[16] = { 0 };
    cc.rgbResult = RGB(255, 255, 255); 
    cc.lpCustColors = custColors;
    cc.Flags = CC_FULLOPEN | CC_RGBINIT;
    if (ChooseColor(&cc))
    {
        SendMessage(editWidget, EM_SETBKGNDCOLOR, FALSE, cc.rgbResult);
    }
}

void ChangeFont()
{
    CHARFORMAT2 cf;
    cf.cbSize = sizeof(CHARFORMAT2);
    CHOOSEFONT cfDialogParams = { 0 };
    LOGFONT lf = { 0 };

    cfDialogParams.lStructSize = sizeof(CHOOSEFONT);
    cfDialogParams.hwndOwner = editWidget;
    cfDialogParams.lpLogFont = &lf;
    cfDialogParams.Flags = CF_INITTOLOGFONTSTRUCT | CF_EFFECTS | CF_SCREENFONTS;

    if (ChooseFont(&cfDialogParams)) {
        lstrcpy(cf.szFaceName, lf.lfFaceName);
        cf.yHeight = lf.lfHeight * 20;
        cf.dwEffects = 0;
        cf.dwMask = CFM_FACE | CFM_SIZE;
        SendMessage(editWidget, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&cf);
    }

}

void ChangeFontColor() {
    CHOOSECOLOR cc = { 0 };
    ZeroMemory(&cc, sizeof(cc));
    cc.lStructSize = sizeof(cc);
    cc.hwndOwner = editWidget;
    cc.lpCustColors = new COLORREF[16]{ 0 };
    cc.Flags = CC_FULLOPEN | CC_RGBINIT;

    if (ChooseColor(&cc))
    {
        COLORREF chosenColor = cc.rgbResult;

        CHARFORMAT2 cf = { };
        cf.cbSize = sizeof(CHARFORMAT2);
        cf.dwMask = CFM_COLOR;
        cf.crTextColor = chosenColor;

        SendMessage(editWidget, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)(&cf));
    }
}

bool OpenFile(LPCWSTR path)
{
    HANDLE hFile = CreateFile(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        OutputDebugString(L"Unable to open file");
        return false;
    }

    DWORD dwHighFileSize;
    DWORD dwFileSize = GetFileSize(hFile, &dwHighFileSize);
    if (dwFileSize == INVALID_FILE_SIZE) {
        OutputDebugString(L"GetFileSize failed");
        CloseHandle(hFile);
        return false;
    }

    HANDLE hMapFile = CreateFileMapping(hFile, NULL, PAGE_READONLY, dwHighFileSize, dwFileSize, NULL);
    if (hMapFile == NULL) {
        OutputDebugString(L"Unable to create file mapping");
        CloseHandle(hFile);
        return false;
    }

    LPVOID lpMapAddress = MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, dwFileSize);
    if (lpMapAddress == NULL) {
        OutputDebugString(L"Unable to map view of file"); 
        CloseHandle(hMapFile);
        CloseHandle(hFile);
        return false;
    }

    std::wstring name = path;

    name = name.substr(name.find_last_of('\\') + 1);
    std::wstring data = std::wstring((char*)lpMapAddress, (char*)lpMapAddress + dwFileSize);

    LPWSTR fileName = const_cast<LPWSTR>(name.data());
    LPCWSTR fileData = data.c_str();
    LPWSTR filePath = const_cast<LPWSTR>(path);

    CreateTab(fileName, fileData, filePath);

    UnmapViewOfFile(lpMapAddress);
    CloseHandle(hMapFile);
    CloseHandle(hFile);
    return true;
}

bool SaveFile(LPWSTR path, LPCSTR data, int length)
{
    HANDLE hFile = CreateFile(path, GENERIC_WRITE, 0, NULL,
        CREATE_ALWAYS, FILE_FLAG_OVERLAPPED, NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        OutputDebugString(L"Unable to open file");
        return 1;
    }

    OVERLAPPED overlapped = { 0 };
    overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    if (!WriteFileEx(hFile, data, length, &overlapped, WriteCompletionRoutine))
    {
        OutputDebugString(L"Unable to async write");
        CloseHandle(hFile);
        return 1;
    }
    SleepEx(INFINITE, TRUE);

    CloseHandle(overlapped.hEvent);
    CloseHandle(hFile);

    return true;
}

void OpenFolder(HWND hWnd)
{
    BROWSEINFO bi = { 0 };
    bi.hwndOwner = hWnd;
    bi.lpszTitle = L"Choose folder:";
    bi.ulFlags = BIF_NEWDIALOGSTYLE | BIF_RETURNONLYFSDIRS;
    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

    if (pidl != NULL)
    {
        TreeView_DeleteAllItems(treeView);
        wchar_t folderPath[MAX_PATH];
        SHGetPathFromIDList(pidl, folderPath);
        currentDirectory = folderPath;
        PopulateTreeViewWithFiles(treeView, TreeView_GetRoot(treeView), folderPath, 0);
        CoTaskMemFree(pidl);
    }
}

void PopulateTreeViewWithFiles(HWND hTreeView, HTREEITEM hParent, const std::wstring& folderPath, const int& indent)
{
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile((folderPath + L"\\*").c_str(), &findFileData);

    if (hFind == INVALID_HANDLE_VALUE)
        return;

    do
    {
        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            // Пропускаем папки "." и ".."
            if (wcscmp(findFileData.cFileName, L".") != 0 && wcscmp(findFileData.cFileName, L"..") != 0)
            {
                TreeView_SetIndent(hTreeView, indent);
                TVITEM tvItem = { 0 };
                tvItem.mask = TVIF_TEXT | TVIF_CHILDREN | TVIF_PARAM;
                tvItem.pszText = findFileData.cFileName;
                tvItem.cChildren = 1;

                TVINSERTSTRUCT tvInsert = { 0 };
                tvInsert.hParent = hParent;
                tvInsert.item = tvItem;

                HTREEITEM hItem = TreeView_InsertItem(hTreeView, &tvInsert);
                PopulateTreeViewWithFiles(hTreeView, hItem, folderPath + L"\\" + findFileData.cFileName, indent + 30 );
            }
        }
        else
        {
            TreeView_SetIndent(hTreeView, indent);
            // Добавляем файлы в TreeView
            TVITEM tvItem = { 0 };
            tvItem.mask = TVIF_TEXT | TVIF_PARAM;
            tvItem.pszText = findFileData.cFileName;

            TVINSERTSTRUCT tvInsert = { 0 };
            tvInsert.hParent = hParent;
            tvInsert.item = tvItem;

            TreeView_InsertItem(treeView, &tvInsert);
        }
    } while (FindNextFile(hFind, &findFileData) != 0);

    FindClose(hFind);

}

void CALLBACK WriteCompletionRoutine(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped)
{
    if (dwErrorCode == 0)
    {
        OutputDebugString(L"Асинхронная запись завершена. Байт записано: " + dwNumberOfBytesTransfered);
    }
    else
    {
        OutputDebugString(L"Ошибка асинхронной записи: " + dwErrorCode);
    }
}

std::wstring GetPathRelativeToRoot(HWND hTreeView, HTREEITEM hItem) 
{
    std::wstring path;
    TVITEM item = { 0 };
    item.mask = TVIF_TEXT;
    item.hItem = hItem;
    item.cchTextMax = 255;
    wchar_t itemName[256];
    item.pszText = itemName;

    if (TreeView_GetItem(hTreeView, &item)) {
        path = itemName;
    }
    
    HTREEITEM hParent = TreeView_GetParent(hTreeView, hItem);

    if (hParent != nullptr) {
        std::wstring parentPath = GetPathRelativeToRoot(hTreeView, hParent);
        path = parentPath + L"\\" + path;
    }

    return path;
}

void OpenProject(HWND hWnd) {
    OPENFILENAME ofn;
    wchar_t szFileName[1024] = L"";

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = L"Text Files\0*.pro\0";
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn) == TRUE)
    {

        TreeView_DeleteAllItems(treeView);

        std::wstring path = szFileName;
        std::wstring filePath, fileName;
        size_t found = path.find_last_of(L"\\/");
        if (found != std::string::npos) {
            filePath = path.substr(0, found);
            fileName = path.substr(found + 1, path.size() - 1);
        }

        PopulateTreeViewWithFiles(treeView, TreeView_GetRoot(treeView), filePath, 0);
        currentDirectory = filePath;

        currentProjectPath = path;

        found = fileName.find_last_of(L".");
        if (found != std::string::npos) {
            fileName = fileName.substr(0, found);
        }
        
        currentProjectName = fileName;
    }
}

DWORD WINAPI CompileProject(LPVOID lpParam) {

    STARTUPINFO startupInfo = { sizeof(STARTUPINFO) };

    std::wstring directory(currentDirectory.begin(), currentDirectory.end());
    directory += L"\\build";

    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile(directory.c_str(), &findFileData);
    
    if (hFind == INVALID_HANDLE_VALUE)
    {
        CreateDirectoryW(directory.c_str(), NULL);
    }
    else
    {
        std::string directoryRemove(directory.begin(), directory.end());
        directoryRemove = "rd /s /q " + directoryRemove;
         
        OutputDebugString(std::wstring(directoryRemove.begin(), directoryRemove.end()).c_str());
        auto result = std::to_wstring(system(directoryRemove.c_str()));
        result = L'\n' + result + L'\n';
        SendMessage(consoleWidget, EM_SETSEL, (WPARAM)result.data(), (LPARAM)result.size() * sizeof(char));
        SendMessage(consoleWidget, EM_REPLACESEL, 0, (LPARAM)result.data());
        CreateDirectoryW(directory.c_str(), NULL);
    }

    FindClose(hFind);

    directory = L" cd " + directory;
    std::wstring qmake = GetHKeyPath(L"QMAKE");
    if (qmake == L"")
    {
        OutputDebugStringW(L"Invalid qmake path");
        return NULL;
    }

    std::wstring mingwMake = GetHKeyPath(L"MINGW_MAKE");

    if (mingwMake == L"")
    {
        OutputDebugStringW(L"Invalid mingw-make path");
        return NULL;
    }

    std::wstring complierQMake = directory + L" && " + qmake + L" ";
    std::wstring params = L" -spec win32-g++ \"CONFIG+=debug\" \"CONFIG+=qml_debug\" && " + mingwMake;
    std::wstring makeStrW = L" qmake && make | tee output.txt";
    std::wstring compileStrW = complierQMake + currentProjectPath + params + makeStrW;
    std::string compileStr(compileStrW.begin(), compileStrW.end());

    OutputDebugString((compileStrW + L"\n").c_str());
    auto resultCompilation = std::to_wstring(system(compileStr.c_str()));
    resultCompilation = L'\n' + resultCompilation + L'\n';
    SendMessage(consoleWidget, EM_SETSEL, (WPARAM)resultCompilation.c_str(), (LPARAM)resultCompilation.size() * sizeof(char));
    SendMessage(consoleWidget, EM_REPLACESEL, 0, (LPARAM)resultCompilation.c_str());

    ReadLogFile();

    std::wstring exeStrW = currentDirectory + L"\\build\\debug\\" + currentProjectName + L".exe";
    OutputDebugString((exeStrW + L"\n").c_str());

    isProcessCreated = true;
    

    if (CreateProcess(NULL, const_cast<LPWSTR>(exeStrW.c_str()), NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo, &processInfo)) {
        WaitForSingleObject(processInfo.hProcess, INFINITE);
        ShowWindow(stopButton, SW_HIDE);
        CloseHandle(processInfo.hProcess);
        CloseHandle(processInfo.hThread);

        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(resourcesGraphicWidget, &ps);
        ClearGraph(hdc);
    }
    else {
        OutputDebugString(L"exe error: " + GetLastError());
    }

    PostMessage(hMainWindow, WM_USER + 2, 0, 0);

    return 0;
}

DWORD WINAPI SaveFileThread(LPVOID param) {
    SaveFileParams* params = reinterpret_cast<SaveFileParams*>(param);
    SaveFile(params->filePath, params->fileContent, params->fileSize);
    delete params;

    return 0;
}

void CompileProjectCommand() {
    if (currentProjectPath.empty()) {
        return;
    }

    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile((currentDirectory + L"\\*").c_str(), &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        OutputDebugString(L"Failed to list files with error: " + GetLastError());
        return;
    }

    std::vector<std::wstring> fileNames;

    do {
        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            continue;
        }

        fileNames.push_back(findFileData.cFileName);
    } while (FindNextFile(hFind, &findFileData) != 0);

    FindClose(hFind);
    SaveFileCommand();

    std::vector<HANDLE> threadHandles;

    for (const std::wstring& fileName : fileNames) {
        auto it = filePathes.find(fileName);

        if (it != filePathes.end()) {
            auto dataIt = fileData.find(fileName)->second;
            int length = dataIt.size();

            wchar_t* wcharPtr = new wchar_t[length + 1];
            wcscpy_s(wcharPtr, length + 1, dataIt.c_str());

            int bufferSize = WideCharToMultiByte(CP_ACP, 0, wcharPtr, -1, NULL, 0, NULL, NULL);
            char* ansiStr = new char[bufferSize];
            WideCharToMultiByte(CP_ACP, 0, wcharPtr, -1, ansiStr, bufferSize, NULL, NULL);

            LPWSTR path = const_cast<LPWSTR>((*it).second.c_str());

            SaveFileParams* params = new SaveFileParams{ path, ansiStr, length + 1 };

            HANDLE hThread = CreateThread(NULL, 0, SaveFileThread, params, 0, NULL);
            if (hThread == NULL) {
                OutputDebugString(L"Failed to list files with error: " + GetLastError());
            }
            threadHandles.push_back(hThread);
        }
    }

    WaitForMultipleObjects(threadHandles.size(), threadHandles.data(), TRUE, INFINITE);
    isProcessCreated = false;
    for (const auto& hThread : threadHandles) {
        CloseHandle(hThread);
    }
    
    if (сompileThread != NULL)
    {
        StopProjectCommand();
    }

    сompileThread = CreateThread(NULL, 0, CompileProject, NULL, 0, NULL);
    if (сompileThread == NULL) {
        OutputDebugString(L"Failed to create thread for compilation. Error code: " + GetLastError());
        return;
    }


    graphicThread = CreateThread(NULL, 0, UpdateAndDraw, (LPVOID)resourcesGraphicWidget, 0, NULL);
    if (graphicThread == NULL) {
        OutputDebugString(L"Failed to create thread for compilation. Error code: " + GetLastError());
        return;
    }

    SetPriorityClass(сompileThread, THREAD_PRIORITY_TIME_CRITICAL);
    
    ShowWindow(stopButton, SW_SHOW);
}

void ReadLogFile() {
    HANDLE hFile = CreateFileW((currentDirectory + L"\\build\\output.txt").c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile != INVALID_HANDLE_VALUE) {
        DWORD fileSize = GetFileSize(hFile, NULL);

        if (fileSize == INVALID_FILE_SIZE && GetLastError() != NO_ERROR) {
            CloseHandle(hFile);
            return;
        }

        content.resize(fileSize + 1);
        DWORD bytesRead;

        if (ReadFile(hFile, content.data(), fileSize, &bytesRead, NULL)) {
            SetWindowTextA(consoleWidget, content.data());
        }
        else {
            return;
        }
        CloseHandle(hFile);
        return;
    }
    else {
        return ;
    }
}

void StopProjectCommand() {
    TerminateThread(сompileThread, 0);
    if (isProcessCreated) {
        TerminateProcess(processInfo.hProcess, 0);
        TerminateThread(processInfo.hThread, 0);
        TerminateThread(graphicThread, 0);
    }
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(resourcesGraphicWidget, &ps);
    ClearGraph(hdc);
    ShowWindow(stopButton, SW_HIDE);
}

void ClearGraph(HDC hdc)
{
    RECT rect;
    GetClientRect(resourcesGraphicWidget, &rect);
    HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
    FillRect(hdc, &rect, hBrush);

}

void DrawGraph(HDC hdc, int memoryUsage)
{
    int height = 200 - memoryUsage;
    if (height < 5)
    {
        graphicScale++;
        return;
    }
    Rectangle(hdc, 50, 150, 100, height);
    RECT rect = { 50, 150, 100, height };
    HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 70));
    FillRect(hdc, &rect, hBrush);
}

DWORD WINAPI UpdateAndDraw(LPVOID lpParam)
{
    HWND hWnd = (HWND)lpParam;
    while (true)
    {
        PROCESS_MEMORY_COUNTERS_EX pmc;

        if (processInfo.hProcess != NULL && GetProcessMemoryInfo(processInfo.hProcess, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc)))
        {
            HDC hdc = GetDC(hWnd);
            ClearGraph(hdc);
            DrawAxes(hdc, 200, 200, 50, 50, 1024 * 10 * graphicScale);

            if (200 - (pmc.WorkingSetSize / (1024 * 10 * graphicScale)) < 5)
            {
                graphicScale++;
                continue;
            }

            DrawGraph(hdc, pmc.WorkingSetSize / (1024 * 10 * graphicScale));
            ReleaseDC(hWnd, hdc);
        }

        Sleep(100);
    }
}

void DrawAxes(HDC hdc, int width, int height, int maxX, int maxY, int axesMaxValue) {
    MoveToEx(hdc, 25, 150, nullptr);
    LineTo(hdc, 25, 5);

    MoveToEx(hdc, 25, 150, nullptr);
    LineTo(hdc, 150, 150);

    int stepY = 150 / 5;
    int axesValue = axesMaxValue / 5;

    for (int i = 150; i >= 0; i -= stepY, axesValue += axesMaxValue / 5) {
        MoveToEx(hdc, 22, i, nullptr);
        LineTo(hdc, 28, i);
        std::wstring label = std::to_wstring(axesValue);
        TextOut(hdc, 0, i, label.c_str(), label.length());
    }
}

std::wstring GetHKeyPath(const wchar_t* valueName) {
    HKEY hKey;
    const wchar_t* subkey = L"Software\\IDETools";
    DWORD valueType;
    wchar_t data[MAX_PATH];

    if (RegOpenKeyEx(HKEY_CURRENT_USER, subkey, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD dataSize = MAX_PATH * sizeof(wchar_t);

        if (RegQueryValueEx(hKey, valueName, NULL, &valueType, (LPBYTE)data, &dataSize) == ERROR_SUCCESS) {
            if (valueType == REG_SZ) {
                return data;
            }
        }

        RegCloseKey(hKey); 
    }

    return L""; 
}

void SetHKeyPath(const wchar_t* valueName, const wchar_t* valueData)
{
    HKEY hKey;
    const wchar_t* subkey = L"Software\\IDETools";
    LONG result = RegCreateKeyEx(HKEY_CURRENT_USER, subkey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL);

    if (result == ERROR_SUCCESS) {
        result = RegSetValueEx(hKey, valueName, 0, REG_SZ, (const BYTE*)valueData, (wcslen(valueData) + 1) * sizeof(wchar_t));
        RegCloseKey(hKey);

        if (result == ERROR_SUCCESS) {
            OutputDebugString(L"Success");
        }
        else {
            OutputDebugString(L"Error set value to register.");
        }
    }
    else {
        OutputDebugString(L"Creating error" );
    }
}

void SetQMakePathCommand()
{
    OPENFILENAME ofn;
    wchar_t szFileName[1024] = L"";

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = L"All Files\0*.*\0";;
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn) == TRUE)
    {
        SetHKeyPath(L"QMAKE", szFileName);
        SetWindowTextW(qmakePath, GetHKeyPath(L"QMAKE").c_str());
    }
}

void SetMingwMakePathCommand()
{
    OPENFILENAME ofn;
    wchar_t szFileName[1024] = L"";

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = L"All Files\0*.*\0";;
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn) == TRUE)
    {
        SetHKeyPath(L"MINGW_MAKE", szFileName);
        SetWindowTextW(mingwMakePath, GetHKeyPath(L"MINGW_MAKE").c_str());
    }
}

void OpenBuildSettingsCommand(HWND hWnd)
{
    hwndBuildSettings = CreateWindow(L"BuildSettingsWindowClass", L"Build Settings Window", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 400, nullptr, nullptr, nullptr, nullptr);

    HWND qmakePathLable = CreateWindow(L"static", L"qmake:", WS_VISIBLE | WS_CHILD | SS_NOTIFY, 
        10, 50, 100, 20, hwndBuildSettings, NULL, NULL, NULL);
    HWND mingwMakePathLable = CreateWindow(L"static", L"mingw_make", WS_VISIBLE | WS_CHILD | SS_NOTIFY,
        10, 100, 100, 20, hwndBuildSettings, NULL, NULL, NULL);

    qmakePath = CreateWindow(L"static", GetHKeyPath(L"QMAKE").c_str(), WS_VISIBLE | WS_CHILD | SS_NOTIFY,
        100, 50, 250, 20, hwndBuildSettings, NULL, NULL, NULL);
    mingwMakePath = CreateWindow(L"static", GetHKeyPath(L"MINGW_MAKE").c_str(), WS_VISIBLE | WS_CHILD | SS_NOTIFY,
        100, 100, 250, 20, hwndBuildSettings, NULL, NULL, NULL);

    HWND changeQMakePath = CreateWindow(L"button", L"Change path", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        360, 50, 100, 20, hwndBuildSettings, (HMENU)CHANGE_QMAKE_PATH_COMMAND, GetModuleHandle(nullptr), NULL);
    HWND changeMingwMakePath = CreateWindow(L"button", L"Change path", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        360, 100, 100, 20, hwndBuildSettings, (HMENU)CHANGE_MINGW_MAKE_PATH_COMMAND, GetModuleHandle(nullptr), NULL);

    ShowWindow(hwndBuildSettings, SW_SHOWNORMAL);
}

LRESULT CALLBACK BuildSettingsProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case CHANGE_QMAKE_PATH_COMMAND:
            SetQMakePathCommand();
            break;
        case CHANGE_MINGW_MAKE_PATH_COMMAND:
            SetMingwMakePathCommand();
            break;
        case WM_CLOSE:
            if (hWnd == hwndBuildSettings)
                DestroyWindow(hWnd);
            break;
        }
        break;

    case WM_DESTROY:
        if (hWnd == hwndBuildSettings)
            hwndBuildSettings = nullptr;
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
