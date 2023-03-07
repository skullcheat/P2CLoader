#include "includes.h"

int main()
{
	WSADATA wsa_data;
	SOCKADDR_IN addr;

	int wsa = WSAStartup(MAKEWORD(2, 2), &wsa_data);
	if (wsa != 0)
	{
		WSACleanup();
		return 0;
	}

	const auto server = socket(AF_INET, SOCK_STREAM, 0);

	InetPton(AF_INET, ip, &addr.sin_addr.s_addr);

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);

	int con = connect(server, reinterpret_cast<SOCKADDR*>(&addr), sizeof(addr));
	if (con != 0)
	{
		closesocket(server);
		WSACleanup();
		return 0;
	}

	printf("connected\n");

	//program
	int pid = getpid();
	string license, buffer;

	//get license
	printf("enter your license: ");
	cin >> license;

	//get hwid
	query();
	hwid += m_CPU + m_ComputerName + m_Physical;
	hwid.erase(remove_if(hwid.begin(), hwid.end(), isspace), hwid.end());

	//prepare and send initialization data and make encryption key
	string preInit = "initcheck__" + hwid + "__" + version + "__" + license;

	string akey = gen_random(32, pid);
	string aiv = gen_random(16, pid);

	string initData = security::encrypt(preInit, akey, aiv);
	buffer = initData + "__" + to_string(pid * 3 - 796);

	send(server, buffer.c_str(), 2048, 0);

	//receive version check and validate
	buffer = recvDec(server, akey, aiv);
	if (buffer == "badinitcheck")
	{
		printf("request took too long\n");
		closesocket(server);
		WSACleanup();
		system("pause");
	}
	else if (buffer == "goodver")
	{
		//receive license and ban check
		buffer = recvDec(server, akey, aiv);
		if (buffer == "hwidban")
		{
			printf("you have been banned from using our services\n");
			closesocket(server);
			WSACleanup();
			system("pause");
		}
		else if (buffer == "invalid" || buffer == "expired")
		{
			printf("your license is invalid or expired\n");
			closesocket(server);
			WSACleanup();
			system("pause");
		}
		else if (buffer == "badhwid")
		{
			printf("your license is bound to a different hwid\n");
			closesocket(server);
			WSACleanup();
			system("pause");
		}
		else
		{
			printf("sucessfully logged in\n");
			Sleep(1337);

			system("cls");
			program();

			closesocket(server);
			WSACleanup();
			system("pause");
		}
	}
	else
	{
		printf("invalid version\n");
		closesocket(server);
		WSACleanup();
		system("pause");
	}

	closesocket(server);
	WSACleanup();
	return 0;
}