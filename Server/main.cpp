#include "..\request.h"
#include "stdafx.h"
#include "Server.h"

int main()
{
	cout << "Chat聊天系统服务端1.0" << endl;
	auto server = Server::Create(9999);
	if (server == nullptr)
	{
		cout << "启动失败" << endl;
		return 1;
	}
	return server->StartServer();
}