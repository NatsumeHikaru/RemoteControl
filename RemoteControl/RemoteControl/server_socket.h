#pragma once

#include "pch.h"
#include "framework.h"

static bool failure = FALSE;
static bool success = TRUE;
static int buffer_size = 4096;

class packet {
public:
	packet() : head(0), length(0), cmd(0), sum(0) {}
	packet(const packet& pack) {
		head = pack.head;
		length = pack.length;
		cmd = pack.cmd;
		str_data = pack.str_data;
		sum = pack.sum;
	}
	packet(const BYTE* pdata, size_t& size) {
		size_t i = 0;
		for (; i < size; ++i) {
			if (*(WORD*)(pdata + i) == 0xFEFF) { // find packet head
				head = *(WORD*)(pdata + i);
				i += 2;
				break;
			}
		}
		if (i + 8 > size) {
			size = 0;
			return;
		}
		length = *(WORD*)(pdata + i);
		i += 4; // read length

		if (length + i > size) { // data is too big
			size = 0;
			return;
		}

		cmd = *(WORD*)(pdata + i);
		i += 2;
		if (length > 4) {
			str_data.resize(length - 2 - 2);
			memcpy((void*)str_data.c_str(), pdata + i, length - 4);
			i += length - 4;
		}
		sum = *(WORD*)(pdata + i);
		i += 2;
		WORD tmp = 0;
		for (int j = 0; j < str_data.length(); ++j) {
			tmp += BYTE(str_data[i]) & 0xFF;
		}
		if (sum == tmp) {
			size = i;
			return;
		}
		size = 0;
	}
	~packet() {}
	packet& operator=(const packet& W) {
		if (this == &W) return *this;
		head = W.head;
		length = W.length;
		cmd = W.cmd;
		str_data = W.str_data;
		sum = W.sum;
	}

	WORD head; // 0x FE FF
	DWORD length; // packet length,from command begin to sum check end
	WORD cmd; // command 
	std::string str_data; // packet data
	WORD sum; // sum check
};

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
	packet m_packet;
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


