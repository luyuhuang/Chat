#pragma once
#include "stdafx.h"

class Client
{
public:
	static Client *Create(const string &userName, const string &addr);
	int GetResponse();
	void StartClient();
	void release();

private:
	Client();

	bool m_isChatRequset;
	string m_rtUserName;
	SOCKET m_socket;
	mutex m_puts;
	mutex m_field;
};