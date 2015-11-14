#include "Server.h"

Server::Server()
{
}

int Server::StartServer()
{
	Client c = Client(1);
	thread client(&Server::ClientThread, this, c);
	

	return 0;
}

int Server::ClientThread(const Client & client)
{
	return 0;
}

