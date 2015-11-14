#pragma once
#include "stdafx.h"

class Client
{
public:
	Client(SOCKET socket, sockaddr_in addr, string name = "");

	void SetName(const string &name) { m_name = name; }
	string GetName() { return m_name; }
	SOCKET GetSocket() { return m_socket; }
	sockaddr_in GetAddr() { return m_addr; }

private:
	SOCKET m_socket;
	string m_name;
	sockaddr_in m_addr;
	Client *m_connenter;
};
