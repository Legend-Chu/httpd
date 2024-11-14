#include <iostream>
#include <stdio.h>
#include <WinSock2.h>
#pragma comment(lib, "WS2_32.lib")

using namespace std;

class Solution
{
private:
    void error_die(const char *str)
    {
        perror(str);
        exit(1);
    }

public:
    // 参数port表示端口
    int startup(unsigned short *port)
    {
        // 1 网络通讯初始化
        WSADATA data;
        int ret = WSAStartup(MAKEWORD(1, 1), &data);
        if (ret)
        {
            error_die("WSAStartup");
        }
        // 2 套接字初始化
        int server_socket = socket(PF_INET, SOCK_STREAM, 0);
        if (server_socket == -1)
        {
            error_die("套接字");
        }

        // 3 设置端口复用
        int opt = 1;
        ret = setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt));
        if (ret == -1)
        {
            error_die("setsockopt");
        }
        // 4 配置服务器端的网络地址
        struct sockaddr_in server_addr;
        memset(&server_socket, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        // 配置端口
        server_addr.sin_port = htons(*port);
        // 配置IP地址
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

        // 5 绑定套接字
        int bind_socket = bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
        if (bind_socket < 0)
        {
            error_die("bind");
        }

        int nameLen = sizeof(server_addr);
        // 动态分配端口
        if (*port == 0)
        {
            if (getsockname(server_socket, (struct sockaddr *)&server_addr, &nameLen) < 0)
            {
                error_die("getSockName");
            }
            *port = server_addr.sin_port;
        }

        // 6 创建监听队列
        if (listen(server_socket, 5) < 0)
        {
            error_die("listen");
        }

        return server_socket;
    }
};

int main(void)
{

    Solution s;
    // 端口长度 = unsigned short的长度 = 0-65553
    unsigned short port = 8080;
    int server_sock = s.startup(&port);
    cout << "httpd服务已启动，正在监听" << port << "端口" << endl;
    return 0;
}