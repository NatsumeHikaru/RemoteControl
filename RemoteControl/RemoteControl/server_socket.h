#pragma once

#include "pch.h"
#include "framework.h"

static bool failure = FALSE;
static bool success = TRUE;

class server_socket
{
private:
	server_socket& operator=(const server_socket& W) {	 }
	server_socket(const server_socket& ss) {
		m_socket = ss.m_socket;
		m_client = ss.m_client;
	}
	server_socket() {
		m_client = INVALID_SOCKET;
		if (init_sock_env() == failure) {
			MessageBox(NULL, _T("几把的，初始化失败"), _T("INIT ERROR"), MB_OK | MB_ICONERROR);
			exit(0);
		}
		m_socket = socket(PF_INET, SOCK_STREAM, 0);
	}
	~server_socket() {
		closesocket(m_socket);
		WSACleanup();
	}

	SOCKET m_socket, m_client;
	bool init_sock_env();
	static server_socket* m_instance;

	static void release_instance();

	class helper {
	public:
		helper() {
			server_socket::get_instance();
		}
		~helper() {
			server_socket::release_instance();
		}
	};
	static helper m_helper;

public:
	static server_socket* get_instance();

	bool init_socket();
	bool accept_client();
	int deal_cmd();
	bool send_msg(const char* data, int size);
};


