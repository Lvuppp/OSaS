// OSaS.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "prototypes.h"
#include "fileHandlers.h"

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
        std::cerr << "Failed to open directory: " << GetLastError() << std::endl;
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
            std::cerr << "ReadDirectoryChangesW failed: " << GetLastError() << std::endl;
            break;
        }

        // Wait for the completion of the overlapped operation
        DWORD waitResult = WaitForSingleObject(directoryHandle, INFINITE);

        if (waitResult == WAIT_OBJECT_0) {
            std::wcout << L"Directory changed." << std::endl;
            // TODO: Handle the directory change as needed
        }
        else {
            std::cerr << "WaitForSingleObject failed: " << GetLastError() << std::endl;
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

     hMainWindow = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hMainWindow)
    {
        return FALSE;
    }

    ShowWindow(hMainWindow, nCmdShow);
    UpdateWindow(hMainWindow);

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
        MoveWindow(editWidget, 302, 31, modifiedWidth - 300, modifiedHeight, TRUE);
        MoveWindow(treeView, 1, 31, 300, modifiedHeight, TRUE);

        break;
    case WM_USER + 1:
        TreeView_DeleteAllItems(treeView);
        PopulateTreeViewWithFiles(treeView,TreeView_GetRoot(treeView), currentDirectory, 0 );
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

    editWidget = CreateWindowEx(
        0,
        MSFTEDIT_CLASS, L"",
        WS_BORDER | WS_CHILD | WS_VISIBLE | ES_MULTILINE | WS_VSCROLL | WS_TABSTOP,
        302, 30, 400, 600,
        hWnd, (HMENU)IDC_RICHEDIT, hInst, NULL
    );

    treeView = CreateWindowEx(
        0,
        WC_TREEVIEW,
        L"",
        WS_VISIBLE | WS_CHILD | WS_BORDER | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_SHOWSELALWAYS | TVS_HASLINES,
        1, 30, 300, 600,
        hWnd,
        (HMENU)IDC_TREEVIEW,
        hInst,
        NULL); 

    wchar_t text[] = L"Root";
    TVITEM tvItem = { 0 };
    tvItem.mask = TVIF_TEXT;
    tvItem.pszText = text;
    TVINSERTSTRUCT tvInsert = { 0 };
    tvInsert.hParent = TVI_ROOT;
    tvInsert.item = tvItem;

    MonitorDirectoryChanges();

    SendMessage(editWidget, EM_SETEVENTMASK, 0, ENM_CHANGE);
}

void WinMenuCreation(HWND hWnd) {
    HMENU fileMenu = CreateMenu();
    HMENU fileSettings = CreateMenu();
    HMENU fileSubMenu = CreateMenu();
    HMENU settingsSubMenu = CreateMenu();

    AppendMenu(fileSubMenu, MF_STRING, OPEN_FOLDER_COMMAND, L"Open folder");
    AppendMenu(fileSubMenu, MF_STRING, NEW_FILE_COMMAND, L"New File");
    AppendMenu(fileSubMenu, MF_STRING, OPEN_FILE_COMMAND, L"Open File");
    AppendMenu(fileSubMenu, MF_STRING, SAVE_FILE_COMMAND, L"Save File");
    AppendMenu(fileSubMenu, MF_STRING, CLOSE_FILE_COMMAND, L"Close");

    AppendMenu(settingsSubMenu, MF_STRING, CHANGE_BG_COLOR, L"Bg");
    AppendMenu(settingsSubMenu, MF_STRING, CHANGE_FONT, L"Font");
    AppendMenu(settingsSubMenu, MF_STRING, CHANGE_FONT_COLOR, L"Font color");
    AppendMenu(settingsSubMenu, MF_STRING, HIGHTLIGHT_TEXT, L"Highlight text");

    AppendMenu(fileMenu, MF_POPUP, (UINT_PTR)fileSubMenu, L"File");
    AppendMenu(fileMenu, MF_POPUP, (UINT_PTR)settingsSubMenu, L"Settings");

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

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = L"Text Files\0*.cpp;*.h\0";;
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

        SendMessage(editWidget, EM_SETCHARFORMAT, SCF_ALL, reinterpret_cast<LPARAM>(&cf));
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
        std::cerr << "GetFileSize failed";
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

    // Ожидание завершения асинхронной операции
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

        // Освобождаем память, выделенную для pidl
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
                // Добавляем папку в TreeView
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
    // Обработка завершения асинхронной операции записи
    if (dwErrorCode == 0)
    {
        std::cout << "Асинхронная запись завершена. Байт записано: " << dwNumberOfBytesTransfered << std::endl;
    }
    else
    {
        std::cerr << "Ошибка асинхронной записи: " << dwErrorCode << std::endl;
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

    // Получаем имя текущего элемента
    if (TreeView_GetItem(hTreeView, &item)) {
        path = itemName;
    }

    // Получаем родительский элемент
    HTREEITEM hParent = TreeView_GetParent(hTreeView, hItem);

    // Если у элемента есть родитель, рекурсивно добавляем его к пути
    if (hParent != nullptr) {
        std::wstring parentPath = GetPathRelativeToRoot(hTreeView, hParent);
        path = parentPath + L"\\" + path;
    }

    return path;
}