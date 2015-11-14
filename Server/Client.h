#pragma once
#include "stdafx.h"

class Client
{
public:
	Client(SOCKET socket, string name = "");

private:
	SOCKET m_socket;
	string m_name;
};
