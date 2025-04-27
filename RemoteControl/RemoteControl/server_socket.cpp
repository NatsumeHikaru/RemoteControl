#include "pch.h"
#include "server_socket.h"

server_socket* server_socket::m_instance = NULL;
server_socket::helper server_socket::m_helper;

server_socket* pserver = server_socket::get_instance();

// class : server_socket
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
	if (m_client == -1) return -1;
	char* buffer = new char[buffer_size];
	memset(buffer, 0, buffer_size);
	size_t index = 0;
	while (true) {
		size_t len = recv(m_client, buffer + index, buffer_size - index, 0);
		if (len < 0) return -1;
		m_packet = packet((BYTE*)buffer,len);
		index += len;
		len = index;
		if (len > 0) {
			memmove(buffer, buffer + len, buffer_size - len);
			index -= len;
			return m_packet.cmd;
		}
	}
	return -1;
}

bool server_socket::send_msg(const char* data, int size) {
	return send(m_client, data, size, 0) > 0;
}

bool server_socket::send_msg(packet& pack) {
	if (m_client == -1) return failure;
	return send(m_client, pack.get_data(), pack.get_size(), 0) > 0;
}

bool server_socket::get_file_path(std::string& path_str) {
	if (m_packet.cmd == 2) {
		path_str = m_packet.str_data; // get file list
		return success;
	}
	return failure;
}

// private
server_socket::server_socket(const server_socket& ss) {
	m_socket = ss.m_socket;
	m_client = ss.m_client;
}

server_socket::server_socket() {
	m_client = INVALID_SOCKET;
	if (init_sock_env() == failure) {
		MessageBox(NULL, _T("几把的，初始化失败"), _T("INIT ERROR"), MB_OK | MB_ICONERROR);
		exit(0);
	}
	m_socket = socket(PF_INET, SOCK_STREAM, 0);
}

server_socket::~server_socket() {
	closesocket(m_socket);
	WSACleanup();
}

// class : packet
// public
packet::packet(WORD ncmd, const BYTE* pdata, size_t size) {
	head = 0xFEFF;
	length = size + 4;
	cmd = ncmd;
	str_data.resize(size);
	memcpy((void*)str_data.c_str(), pdata, size);
	sum = 0;
	for (size_t i = 0; i < str_data.size(); ++i)
		sum += BYTE(str_data[i]) & 0xFF;
}

packet::packet(const packet& pack) {
	head = pack.head;
	length = pack.length;
	cmd = pack.cmd;
	str_data = pack.str_data;
	sum = pack.sum;
}

packet::packet(const BYTE* pdata, size_t& size) {
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
		tmp += BYTE(str_data[j]) & 0xFF;
	}
	if (sum == tmp) {
		size = i;
		return;
	}
	size = 0;
}

packet& packet::operator=(const packet& W) {
	if (this == &W) return *this;
	head = W.head;
	length = W.length;
	cmd = W.cmd;
	str_data = W.str_data;
	sum = W.sum;
}

int packet::get_size() {
	return length + 6;
}

const char* packet::get_data() {
	str_out.resize(get_size());
	BYTE* pdata = (BYTE*)str_out.c_str();
	*(WORD*)pdata = head; pdata += 2;
	*(WORD*)(pdata) = length; pdata += 4;
	*(WORD*)pdata = cmd; pdata += 2;
	memcpy(pdata, str_data.c_str(), str_data.size()); pdata += str_data.size();
	*(WORD*)pdata = sum;
	return str_out.c_str();
}