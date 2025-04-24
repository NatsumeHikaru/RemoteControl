#pragma once

#include "pch.h"
#include "framework.h"

static bool failure = FALSE;
static bool success = TRUE;
static int buffer_size = 4096;

class packet {
public:
	packet() : head(0), length(0), cmd(0), sum(0) {}
	packet(WORD ncmd, const BYTE* pdata, size_t size);
	packet(const packet& pack);
	packet(const BYTE* pdata, size_t& size);
	~packet() {}
	packet& operator=(const packet& W);

	int get_size();
	const char* get_data();

	WORD head; // 0x FE FF
	DWORD length; // packet length,from command begin to sum check end
	WORD cmd; // command 
	std::string str_data; // packet data
	WORD sum; // sum check
	std::string str_out;
};

class server_socket
{
private:
	server_socket& operator=(const server_socket& W);
	server_socket(const server_socket& ss);
	server_socket();
	~server_socket();

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
	bool send_msg(packet& pack);
};
