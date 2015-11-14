#pragma once
#include "stdafx.h"
#include "Client.h"

class Server
{
public:
	static Server *Create(u_short port);
	void release();

	int StartServer();
	int ClientThread(Client *pClient);

private:
	Server();
	~Server();

	SOCKET m_socket;
	sockaddr_in m_addr;
	vector<Client*> m_clientList;
};
