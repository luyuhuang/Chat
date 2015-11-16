#include "stdafx.h"
#include "Client.h"

using namespace std;

int main()
{
	cout << "Chat聊天系统1.0" << endl;
	cout << "请输入用户名注册:" << endl;

	string userName;
	cin >> userName;

	auto client = Client::Create(userName, "heyoh.wicp.net:14034");
	if (client == nullptr)
	{
		cout << "启动失败" << endl;
	}
	client->StartClient();
	client->release();

	system("pause");
	return 0;

}