#include <iostream>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib") // 加载 Winsock 库

using namespace std;
int startup(unsigned short *port)
{
    // 1. 初始化 Winsock 库
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0)
    {
        cerr << "WSAStartup failed: " << result << endl;
        return -1;
    }

    // 2. 创建套接字
    int server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_sock == INVALID_SOCKET)
    {
        cerr << "Socket creation failed: " << WSAGetLastError() << endl;
        WSACleanup();
        return -1;
    }

    // 3. 配置服务器地址结构
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 监听所有网络接口
    server_addr.sin_port = htons(*port);             // 绑定到指定端口

    // 4. 绑定套接字
    result = bind(server_sock, (sockaddr *)&server_addr, sizeof(server_addr));
    if (result == SOCKET_ERROR)
    {
        cerr << "Bind failed: " << WSAGetLastError() << endl;
        closesocket(server_sock);
        WSACleanup();
        return -1;
    }

    // 5. 设置监听队列，等待客户端连接
    result = listen(server_sock, SOMAXCONN);
    if (result == SOCKET_ERROR)
    {
        cerr << "Listen failed: " << WSAGetLastError() << endl;
        closesocket(server_sock);
        WSACleanup();
        return -1;
    }

    // 6. 动态分配端口，返回绑定的端口号
    sockaddr_in addr;
    int addr_len = sizeof(addr);
    result = getsockname(server_sock, (sockaddr *)&addr, &addr_len);
    if (result == SOCKET_ERROR)
    {
        cerr << "Getsockname failed: " << WSAGetLastError() << endl;
        closesocket(server_sock);
        WSACleanup();
        return -1;
    }
    *port = ntohs(addr.sin_port); // 将端口号从网络字节序转回主机字节序

    return server_sock;
}

int main()
{
    while (true)
    {
        unsigned short port = 8080; // 你可以设置一个固定端口或者将端口号设为0，操作系统会动态分配
        int server_sock = startup(&port);
        if (server_sock == -1)
        {
            return -1; // 启动失败
        }

        cout << "HTTP服务器已启动，正在监听端口 " << port << "..." << endl;
        // 7. 等待并接受客户端连接
        sockaddr_in client_addr;
        int client_addr_len = sizeof(client_addr);
        SOCKET client_sock = accept(server_sock, (sockaddr *)&client_addr, &client_addr_len);
        if (client_sock == INVALID_SOCKET)
        {
            cerr << "Accept failed: " << WSAGetLastError() << endl;
            continue;
        }

        cout << "收到来自 " << inet_ntoa(client_addr.sin_addr) << " 的连接" << endl;

        // 8. 处理客户端请求
        const char *http_response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Connection: close\r\n\r\n"
            "Hello, World!\n";

        send(client_sock, http_response, strlen(http_response), 0);
        closesocket(client_sock); // 关闭客户端连接
    }

    // 9. 关闭服务器套接字
    closesocket(server_sock);
    WSACleanup(); // 清理 Winsock
    return 0;
}
