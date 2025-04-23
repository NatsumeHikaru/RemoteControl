#include "pch.h"
#include "server_socket.h"

server_socket* server_socket::m_instance = NULL;
server_socket::helper server_socket::m_helper;

server_socket* pserver = server_socket::get_instance();

// private
bool server_socket::init_sock_env() {
	WSADATA data;
	if (WSAStartup(MAKEWORD(1, 1), &data) != 0) {
		return failure;
	}
	return success;
}

void server_socket::release_instance() {
	if (m_instance != NULL) {
		server_socket* tmp = m_instance;
		m_instance = NULL;
		delete tmp;
	}
}

// public
server_socket* server_socket::get_instance() {
	if (m_instance == NULL)
		m_instance = new server_socket();
	return m_instance;
}

bool server_socket::init_socket(){

	if (m_socket == -1) return failure;
	// init server address
	sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = PF_INET;
	serv_addr.sin_addr.S_un.S_addr = INADDR_ANY; // listen all ip address
	serv_addr.sin_port = htons(9555); // connect port
	// bind
	if (bind(m_socket, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
		return failure;
	// listen
	if (listen(m_socket, 1) == -1)
		return failure;

	return success;
}

bool server_socket::accept_client() {
	sockaddr_in client_addr;
	int cli_size = sizeof(client_addr);
	m_client = accept(m_socket, (sockaddr*)&client_addr, &cli_size);
	if (m_client == -1) return failure;
	return success;
}

int server_socket::deal_cmd() {
	if (m_client == -1) return failure;
	char buffer[1025] = "";
	while (true) {
		int len = recv(m_client, buffer, sizeof(buffer), 0);
		if (len < 0) return -1;
		// todo
	}
}

bool server_socket::send_msg(const char* data, int size) {
	return send(m_client, data, size, 0) > 0;
}