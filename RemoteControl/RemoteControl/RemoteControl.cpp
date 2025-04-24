// RemoteControl.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include "framework.h"
#include "server_socket.h"
#include "direct.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// #pragma comment(linker,"/subsystem:windows /entry:WinMainCRTStartup")
// #pragma comment(linker,"/subsystem:windows /entry:mainCRTStartup")
// #pragma comment(linker,"/subsystem:console /entry:WinMainCRTStartup")
// #pragma comment(linker,"/subsystem:console /entry:mainCRTStartup")


// 唯一的应用程序对象

CWinApp theApp;

std::string get_driver_info() {
    std::string res;
    for (int i = 1; i <= 26; ++i)
        if (_chdrive(i) == 0) {
            if (res.size()) res += ',';
            res += 'A' + i - 1;
        }
    packet pack(1, (BYTE*)res.c_str(), res.size());
    server_socket::get_instance()->send_msg(pack);
    return res;
}

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
             
            // TODO
            /*
            server_socket* p_server = server_socket::get_instance();
            if (p_server->init_socket() == failure) {
                MessageBox(NULL, _T("网络初始化异常，请检查网络状态！"), _T("网络初始化失败"), MB_OK | MB_ICONERROR);
                exit(0);
            }

            int cnt = 0;
            while (server_socket::get_instance() != NULL) {
 
                if (p_server->accept_client() == failure) {
                    if (cnt >= 3) {
                        MessageBox(NULL, _T("多次重试无法连接"), _T("接入用户失败"), MB_OK | MB_ICONERROR);
                        exit(0);
                    }
                    MessageBox(NULL, _T("接入用户失败，正在自动重试"), _T("接入用户失败"), MB_OK | MB_ICONERROR);
                    cnt++;
                }
                int ret = p_server->deal_cmd(); // todo
            }
            */
            int op = 1;
            switch (op) {
            case 1:
                get_driver_info();
            default:
                break;
            }

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
