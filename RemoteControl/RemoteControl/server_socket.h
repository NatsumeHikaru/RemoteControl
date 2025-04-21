#pragma once

#include "pch.h"
#include "framework.h"

static bool failure = FALSE;
static bool success = TRUE;

class server_socket
{
public:
	server_socket() {
		if (init_sock_env() == failure) {
			MessageBox(NULL, _T("几把的，初始化失败"), _T("INIT ERROR"), MB_OK | MB_ICONERROR);
			exit(0);
		}
	}
	~server_socket() {
		WSACleanup();
	}
	bool init_sock_env() {
		WSADATA data;
		if (WSAStartup(MAKEWORD(1, 1), &data) != 0) {
			return failure;
		}
		return success;
	}
};

extern server_socket server;

