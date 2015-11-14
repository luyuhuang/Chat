#include "..\request.h"
#include "stdafx.h"
#include "Server.h"

int main()
{
	auto server = Server::Create(9999);
	server->StartServer();
	server->release();

	system("pause");
	return 0;
}