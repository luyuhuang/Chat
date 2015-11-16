#pragma once
#include "stdafx.h"
#include "Client.h"

class Server
{
public:
	static Server *Create(u_short port);
	void release();

	int Listen();
	int ClientThread(Client *pClient);
	int StartServer();
private:
	Server();
	~Server();

	Client *FindClientByName(const string &name);

	SOCKET m_socket;
	sockaddr_in m_addr;
	vector<Client*> m_clientList;
	mutex m_listMt;
};
