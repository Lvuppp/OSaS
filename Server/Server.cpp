// Server.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "Server.h"
#include <vector>
#include <string>

#pragma comment(lib, "ws2_32.lib")
#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

std::vector<std::pair<SOCKET,int>> sockets;
enum class Operation
{
    SendFile,
    UpdateReceviers,
    CloseHandle,
};


struct Package {
    Operation operation;
    int data = -1;
    int receiverId = -1;
};

int freeID = 1;
void updateInfoConnections(int index);

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 1), &wsaData) != 0) {
        OutputDebugString(L"WSAStartup failed.");
        return 1;
    }
    sockaddr_in serverAddr;
    int sizeOfAdders = sizeof(serverAddr);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(1111);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr); // IP сервера

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        OutputDebugString(L"Failed to create socket.");
        return 1;
    }

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        OutputDebugString(L"Bind failed.");
        return 1;
    }

    if (listen(serverSocket, 5) == SOCKET_ERROR) {
        OutputDebugString(L"Listen failed.");
        return 1;
    }

    OutputDebugString(L"WebSocket server is running. Listening on port 1111");


    while (true) {
        SOCKET clientSocket = accept(serverSocket, (sockaddr*)&serverAddr, &sizeOfAdders);
        if (clientSocket == INVALID_SOCKET) {
            OutputDebugString(L"Accept failed.");
            continue;
        }
        OutputDebugString(L"Accept.");

        if (clientSocket)
        {
            sockets.push_back(std::make_pair(clientSocket, freeID++));
            int socketSize = sockets.size();
            CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)updateInfoConnections, (LPVOID)(socketSize - 1), NULL, NULL);
            std::vector<int> indexes;
            Package package{ Operation::UpdateReceviers ,socketSize, -1 };
            for (size_t i = 0; i < sockets.size(); i++)
            {
                indexes.push_back(sockets[i].second);
            }

            std::vector<char> serializedData(indexes.size() * sizeof(int));
            memcpy(serializedData.data(), indexes.data(), serializedData.size());

            for (int i = 0; i < sockets.size(); i++)
            {
                if (sockets[i].second != 0)
                {
                    send(sockets[i].first, (char*)(&package), sizeof(Package), NULL);
                    send(sockets[i].first, (char*)(&i), sizeof(int), NULL);
                    send(sockets[i].first, serializedData.data(), serializedData.size(), NULL);
                }
            }
        }
    }

    closesocket(serverSocket);
    WSACleanup();
   
    return (int) 0;
}


void updateInfoConnections(int index)
{
    Package package;

    while (true)
    {
        recv(sockets[index].first, (char*)&package, sizeof(Package), NULL);
        
        switch (package.operation)
        {
        case Operation::SendFile:
        {
            std::wstring fileName;
            fileName.resize(package.data);
            std::string file;
            int sizeFile;
            int answer;
            recv(sockets[index].first, (char*)fileName.data(), package.data * sizeof(wchar_t), NULL);
            recv(sockets[index].first, (char*)&sizeFile, sizeof(int), NULL);
            file.resize(sizeFile);
            recv(sockets[index].first, (char*)file.data(), sizeFile, NULL);

            Package receiverPackage{ Operation::SendFile, package.data, index};
            send(sockets[package.receiverId - 1].first, (char*)&receiverPackage, sizeof(Package), NULL);
            send(sockets[package.receiverId - 1].first, (char*)fileName.data(), package.data * sizeof(wchar_t), NULL);
            send(sockets[package.receiverId - 1].first, (char*)&sizeFile, sizeof(int), NULL);
            send(sockets[package.receiverId - 1].first, (char*)file.data(), sizeFile * sizeof(char), NULL);
            
        }
            break;
        case Operation::CloseHandle:
        {
            closesocket(sockets[package.receiverId].first);
            sockets[index].second = 0;
            int socketSize = sockets.size();
            std::vector<int> indexes;
            Package package{ Operation::UpdateReceviers ,socketSize, -1 };
            for (size_t i = 0; i < sockets.size(); i++)
            {
                indexes.push_back(sockets[i].second);
            }

            std::vector<char> serializedData(indexes.size() * sizeof(int));
            memcpy(serializedData.data(), indexes.data(), serializedData.size());

            for (int i = 0; i < sockets.size(); i++)
            {
                if (sockets[i].second != 0)
                {
                    send(sockets[i].first, (char*)(&package), sizeof(Package), NULL);
                    send(sockets[i].first, (char*)(&i), sizeof(int), NULL);
                    send(sockets[i].first, serializedData.data(), serializedData.size(), NULL);
                }
            }
        }
            break;
        default:
            break;
        }

        if (package.operation == Operation::CloseHandle)
        {
            break;
        }
    }
}
