#include "Client.h"

Client::Client(SOCKET socket, string name)
	: m_socket(socket), m_name(name)
{
}
