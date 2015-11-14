#include "Client.h"

Client::Client(SOCKET socket, sockaddr_in addr, string name)
	: m_socket(socket), m_addr(addr), m_name(name), m_connenter(nullptr)
{
}
