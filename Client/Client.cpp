#include "Client.h"

Client::Client()
{}

Client * Client::Create(const string &userName, const string &addr)
{
	WORD sockVer = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVer, &wsaData))
	{
		cout << "WSA初始化失败" << endl;
		return nullptr;
	}

	Client *client = new Client();

	client->m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (client->m_socket == INVALID_SOCKET)
	{
		cout << "套接字初始化失败" << endl;
	}

	int pos = addr.find(":");
	string domain = addr.substr(0, pos);
	int port = atoi(addr.substr(pos + 1).c_str());

	hostent * host = gethostbyname(domain.c_str());
	in_addr ip;
	memcpy(&ip.S_un.S_addr, host->h_addr_list[0], host->h_length);

	sockaddr_in serAddr;
	serAddr.sin_addr.S_un.S_addr = ip.S_un.S_addr;
	serAddr.sin_port = htons(port);
	serAddr.sin_family = AF_INET;

	if (connect(client->m_socket, (LPSOCKADDR)(&serAddr), sizeof(serAddr)) == SOCKET_ERROR)
	{
		cout << "连接失败" << endl;
		delete client;
		return nullptr;
	}
	cout << "链接成功，正在尝试注册用户名" << userName << endl;
	//============注册============================
	REQUEST request;
	request.data.registered.flag = RT_REGISTERED;
	strcpy(request.data.registered.userName, userName.c_str());
	
	send(client->m_socket, request.ruler, sizeof(request.data.registered), 0);

	thread getResponse(&Client::GetResponse, client);
	getResponse.detach();

	return client;
}

int Client::GetResponse()
{
	while (true)
	{
		char dataBuffer[BUFFER_SIZE];
		int ret = recv(m_socket, dataBuffer, BUFFER_SIZE, 0);
		if (ret > 0)
		{
			RESPONSE data;
			for (int i = 0; i < ret; i++)
			{
				data.ruler[i] = dataBuffer[i];
			}
			int flag = *(int*)&dataBuffer[0];

			if (flag == RE_ERROR)
			{
				cout << data.data.error.errorMsg << endl;
			}
		}
	}
	return 0;
}
