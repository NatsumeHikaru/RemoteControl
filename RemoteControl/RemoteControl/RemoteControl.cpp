// RemoteControl.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include "framework.h"
#include "RemoteControl.h"
#include "server_socket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 唯一的应用程序对象

CWinApp theApp;

using namespace std;

int main()
{
    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        // 初始化 MFC 并在失败时显示错误
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: 在此处为应用程序的行为编写代码。
            wprintf(L"错误: MFC 初始化失败\n");
            nRetCode = 1;
        }
        else
        {
             
            // TODO: socket,bind,listen,accept,read,write,close

            SOCKET serv_socket = socket(PF_INET, SOCK_STREAM, 0); // todo : check success or fail
            
            // init server address
            sockaddr_in serv_addr, client_addr;
            memset(&serv_addr, 0, sizeof(serv_addr));
            serv_addr.sin_family = PF_INET;
            serv_addr.sin_addr.S_un.S_addr = INADDR_ANY; // listen all ip address
            serv_addr.sin_port = htons(9555); // connect port
            // bind
            bind(serv_socket, (sockaddr*)&serv_addr, sizeof(serv_addr)); // todo : check return value
            // listen
            listen(serv_socket, 1);
            // accept
            int cli_size = sizeof(client_addr);
            SOCKET client = accept(serv_socket, (sockaddr*)&client_addr, &cli_size);
            // read
            char buffer[1024];
            recv(client, buffer, sizeof(buffer),0);
            // write
            send(client, buffer, sizeof(buffer), 0);
            // close
            closesocket(serv_socket);
            WSACleanup();
        }
    }
    else
    {
        // TODO: 更改错误代码以符合需要
        wprintf(L"错误: GetModuleHandle 失败\n");
        nRetCode = 1;
    }

    return nRetCode;
}
