#pragma once
#include "stdafx.h"

class Client
{
public:
	static Client *Create(const string &userName, const string &addr);
	int GetResponse();

private:
	Client();

	SOCKET m_socket;
	mutex m_print;
};