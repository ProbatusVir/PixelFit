// ServerPackage.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include "WindowsServer.h"
#include "SQLInterface.h"
#include <iostream>

int main()
{
#if defined(_WIN32)

	int selectLan = 1;
	std::cout << "Press 1 for LAN connection, 2 for localhost\n";
	std::cin >> selectLan;
	WindowsServer server(selectLan);
	SQLInterface* sql_interface = SQLInterface::Instance();
	if (server.IPSetupComplete()) {
		server.Start();
	}

	SQLInterface::Destroy();
#elif defined(_linux_)
	std::cout << "Linux operating system \n"


#endif


}


