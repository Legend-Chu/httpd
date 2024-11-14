#include <iostream>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib") // 加载 Winsock 库

using namespace std;

int main()
{
    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    sockaddr_in serverAddr, clientAddr;
    int clientAddrLen = sizeof(clientAddr);

    // 初始化 Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        cout << "Winsock initialization failed!" << endl;
        return 1;
    }

    // 创建套接字
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET)
    {
        cout << "Socket creation failed!" << endl;
        WSACleanup();
        return 1;
    }

    // 配置服务器地址
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(8080);

    // 绑定套接字
    if (bind(serverSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        cout << "Bind failed!" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // 开始监听
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        cout << "Listen failed!" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    cout << "Server is listening on port 8080..." << endl;

    // 接受客户端连接
    clientSocket = accept(serverSocket, (sockaddr *)&clientAddr, &clientAddrLen);
    if (clientSocket == INVALID_SOCKET)
    {
        cout << "Accept failed!" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    cout << "Client connected!" << endl;

    // 清理
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
