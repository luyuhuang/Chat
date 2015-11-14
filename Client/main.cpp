#define _CRT_SECURE_NO_WARNINGS
#include "..\request.h"
#include <Windows.h>
#include <iostream>

#pragma  comment(lib,"ws2_32.lib")

using namespace std;

int main()
{
	WORD sockVer = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVer, &wsaData))
	{
		cout << "WSA³õÊ¼»¯Ê§°Ü" << endl;
		return 1;
	}

	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	sockaddr_in addr;
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	addr.sin_family = AF_INET;
	addr.sin_port = htons(9999);
	connect(sock, (LPSOCKADDR)&addr, sizeof(addr));

	REQUEST request;
	request.data.registered.flag = RT_REGISTERED;
	strcpy(request.data.registered.userName, "Heyoh");

	send(sock, request.ruler, sizeof(request.data.registered), 0);
	Sleep(10);
	RESPONSE response;
	char dataBuffer[BUFFER_SIZE];
	int ret = recv(sock, dataBuffer, BUFFER_SIZE, 0);
	if (ret > 0)
	{
		for (int i = 0; i < ret; i++)
		{
			response.ruler[i] = dataBuffer[i];
		}
		int flag = *(int*)&dataBuffer[0];
		if (flag == RE_REGISTERED_RETURN)
		{
			if (response.data.registeredReturn.isSuccess == 1)
			{
				cout << "×¢²á³É¹¦" << endl;
			}
			else
			{
				cout << "×¢²áÊ§°Ü" << endl;
			}
		}
	}

	closesocket(sock);
	system("pause");
	return 0;

}