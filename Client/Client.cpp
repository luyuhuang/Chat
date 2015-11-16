#include "Client.h"

#define PUTS_LOCK m_puts.lock()
#define PUTS_UNLOCK m_puts.unlock()

Client::Client()
	: m_isChatRequset(false)
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

	thread getResponse(&Client::GetResponse, client);		//新建线程，不断监听来自服务器的消息
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

			if (flag == RE_ERROR)//===============================错误信息=======================================
			{
				PUTS_LOCK; cout << data.data.error.errorMsg << endl; PUTS_UNLOCK;
			}
			else if (flag == RE_CHAT_REQUEST)//======================聊天请求=======================================
			{
				PUTS_LOCK; cout << "\n一位名叫" << data.data.chatRequest.userName << "的好友请求和你聊天，同意(y)还是拒绝(n)？"; PUTS_UNLOCK;
				
				m_field.lock();
				m_isChatRequset = true;
				m_rtUserName = data.data.chatRequest.userName;
				m_field.unlock();
				//char agree;
				//while (true)
				//{
				//	cin >> agree;
				//	if (agree == 'y')
				//	{
				//		PUTS_LOCK; cout << "正在开启与" << data.data.chatRequest.userName << "的通话，这将终止当前的通话" << endl; PUTS_UNLOCK;

				//		REQUEST request;
				//		request.data.agree.flag = RT_AGREE;
				//		request.data.agree.isAgree = 1;
				//		strcpy(request.data.agree.userName, data.data.chatRequest.userName);

				//		send(m_socket, request.ruler, sizeof(request.data.agree), 0);
				//	}
				//	else if (agree == 'n')
				//	{
				//		PUTS_LOCK; cout << "已拒绝" << endl; PUTS_UNLOCK;

				//		REQUEST request;
				//		request.data.agree.flag = RT_AGREE;
				//		request.data.agree.isAgree = 0;
				//		strcpy(request.data.agree.userName, data.data.chatRequest.userName);

				//		send(m_socket, request.ruler, sizeof(request.data.agree), 0);
				//	}
				//}
			}
			else if (flag == RE_AGREE_RETURN)//===========================请求聊天的回执=================================
			{
				PUTS_LOCK;
				if (data.data.agreeReturn.isAgree == 1)
				{
					cout << "对方已同意你的请求，通话已建立" << endl;
				}
				else
				{
					cout << "对方拒绝了你的请求" << endl;
				}
				PUTS_UNLOCK;
			}
			else if (flag == RE_EXIT_CHAT_RETURN)//===========================退出聊天的回执==============================
			{
				PUTS_LOCK; cout << "对方终止了当前通话" << endl; PUTS_UNLOCK;
			}
			else if (flag == RE_RECEIVE)//====================================接收信息====================================
			{
				PUTS_LOCK; cout << data.data.receive.message << endl; PUTS_UNLOCK;
			}
			else if (flag == RE_SEND_LIST)//==================================收到用户列表===============================
			{
				PUTS_LOCK; cout << data.data.sendList.list << endl; PUTS_UNLOCK;
			}
		}
	}
	return 0;
}

void Client::StartClient()
{
	while (true)
	{
		PUTS_LOCK; cout << "\n>>>"; PUTS_UNLOCK;
		string _cmd;
		getline(cin, _cmd);
		stringstream cmd(_cmd);

		string cmdHead;
		cmd >> cmdHead;
		//=========================开始解析命令=================================================
		if (m_isChatRequset)
		{
			while (true)
			{
				if (cmdHead == "y")
				{
					PUTS_LOCK; cout << "正在开启与" << m_rtUserName << "的通话，这将终止当前的通话" << endl; PUTS_UNLOCK;

					REQUEST request;
					request.data.agree.flag = RT_AGREE;
					request.data.agree.isAgree = 1;
					strcpy(request.data.agree.userName, m_rtUserName.c_str());

					send(m_socket, request.ruler, sizeof(request.data.agree), 0);
				}
				else if (cmdHead == "n")
				{
					PUTS_LOCK; cout << "已拒绝" << endl; PUTS_UNLOCK;

					REQUEST request;
					request.data.agree.flag = RT_AGREE;
					request.data.agree.isAgree = 0;
					strcpy(request.data.agree.userName, m_rtUserName.c_str());

					send(m_socket, request.ruler, sizeof(request.data.agree), 0);
				}
			}
		}
		else 
		{
			if (cmdHead == "chat")
			{
				string parameter;
				cmd >> parameter;
				if (parameter == "with")
				{
					string userName;
					cmd >> userName;

					REQUEST request;
					request.data.requestChat.flag = RT_REQUEST_CHAT;
					strcpy(request.data.requestChat.userName, userName.c_str());

					send(m_socket, request.ruler, sizeof(request.data.requestChat), 0);
				}
				else if (parameter == "exit")
				{
					REQUEST request;
					request.data.exitChat.flag = RT_EXIT_CHAT;

					send(m_socket, request.ruler, sizeof(request.data.exitChat), 0);
				}
				else
				{
					cout << "命令输入错误！" << endl;
				}
			}
			else if (cmdHead == "m")
			{
				string message;
				cmd >> message;

				REQUEST request;
				request.data.send.flag = RT_SEND;
				strcpy(request.data.send.message, message.c_str());

				send(m_socket, request.ruler, sizeof(request.data.send), 0);
			}
			else if (cmdHead == "list")
			{
				REQUEST request;
				request.data.getList.flag = RT_GET_LIST;
				
				send(m_socket, request.ruler, sizeof(request.data.getList), 0);
			}
			else if (cmdHead == "exit")
			{
				REQUEST request;
				request.data.exit.flag = RT_EXIT;

				send(m_socket, request.ruler, sizeof(request.data.exit), 0);
				break;
			}
		}
	}
}

void Client::release()
{
	closesocket(m_socket);
	WSACleanup();
	delete this;
}
