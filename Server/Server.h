#pragma once
#include "stdafx.h"
#include "Client.h"

class Server
{
public:
	Server();
	int StartServer();
	int ClientThread(const Client &client);

private:
	SOCKET m_socket;
	sockaddr_in m_addr;
	vector<Client> m_clientList;
};
