#include "Server.h"

Server::Server()
{
}

Server::~Server()
{
	for (auto pClient : m_clientList)
	{
		closesocket(pClient->GetSocket());
		delete pClient;
	}
}

Server * Server::Create(u_short port)
{
	WORD sockVer = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVer, &wsaData))
	{
		cout << "WSA初始化失败" << endl;
		return nullptr;
	}

	Server *server = new Server();

	server->m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server->m_socket == INVALID_SOCKET)
	{
		cout << "套接字初始化失败" << endl;
		delete server;
		return nullptr;
	}

	server->m_addr.sin_addr.S_un.S_addr = INADDR_ANY;
	server->m_addr.sin_family = AF_INET;
	server->m_addr.sin_port = htons(port);
	if (::bind(server->m_socket, (LPSOCKADDR)&server->m_addr, sizeof(server->m_addr)) == SOCKET_ERROR)
	{
		cout << "绑定端口失败" << endl;
		delete server;
		return nullptr;
	}


	return server;
}

void Server::release()
{
	delete this;
}

int Server::StartServer()
{
	if (listen(m_socket, 5) == SOCKET_ERROR)
	{
		cout << "监听失败" << endl;
		return 1;
	}

	while (true)
	{
		SOCKET clientSocket;
		sockaddr_in clientAddr;
		int addrLen = sizeof(clientAddr);

		clientSocket = accept(m_socket, (LPSOCKADDR)&clientAddr, &addrLen);
		if (clientSocket == INVALID_SOCKET)
		{
			cout << "接收失败" << endl;
			return 2;
		}
		Client *pClient = new Client(clientSocket, clientAddr);
		m_clientList.push_back(pClient);
		cout << "接收到来自" << inet_ntoa(clientAddr.sin_addr) << "的链接" << endl;

		thread clientThread(&Server::ClientThread, this, pClient);
		clientThread.detach();
	}

	return 0;
}

int Server::ClientThread(Client *pClient)
{
	while (true)
	{
		char dataBuff[BUFFER_SIZE];
		int ret = recv(pClient->GetSocket(), dataBuff, BUFFER_SIZE, 0);
		if (ret > 0)
		{
			REQUEST data;
			for (int i = 0; i < ret; i++)
			{
				data.ruler[i] = dataBuff[i];
			}

			int flag = *(int*)&dataBuff[0];
			if (flag == RT_REQUEST_CHAT)			//请求聊天
			{
				string userName = data.data.requestChat.userName;
				SOCKET clientSocket = 0;
				for (auto client : m_clientList)
				{
					if (client->GetName() == userName)
					{
						clientSocket = client->GetSocket();
						break;
					}
				}

				if (clientSocket != 0)
				{
					RESPONSE response;
					response.data.chatRequest.flag = RE_CHAT_REQUEST;
					strcpy(response.data.chatRequest.usetName, pClient->GetName().c_str());

					send(clientSocket, response.ruler, sizeof(response.data.chatRequest), 0);
				}
				else
				{
					RESPONSE response;
					response.data.agreeReturn.flag = RE_AGREE_RETURN;
					response.data.agreeReturn.isAgree = 0;

					send(pClient->GetSocket(), response.ruler, sizeof(response.data.agreeReturn), 0);
				}
			}
			else if (flag == RT_REGISTERED)			//注册
			{
				string userName = data.data.registered.userName;
				bool isRepeat = false;	//检测是否重名
				for (auto client : m_clientList)
				{
					if (client->GetName() == userName) isRepeat = true;
				}
				RESPONSE response;
				response.data.registeredReturn.flag = RE_REGISTERED_RETURN;
				if (isRepeat)	//如果重名了
				{
					response.data.registeredReturn.isSuccess = 0;
				}
				else
				{
					response.data.registeredReturn.isSuccess = 1;
					pClient->SetName(userName);
				}
				send(pClient->GetSocket(), response.ruler, sizeof(response.data.registeredReturn), 0);

			}
			else if (flag == RT_AGREE)
			{

			}
		}
	}

	return 0;
}

