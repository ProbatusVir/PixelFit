// ServerPackage.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include "WindowsServer.h"
#include "SQLInterface.h"

int main()
{
#if defined(_WIN32)

	WindowsServer server;
	SQLInterface* sql_server = SQLInterface::Instance();
	if (server.IPSetupComplete()) {
		server.Start();
	}

#elif defined(_linux_)
	std::cout << "Linux operating system \n"


#endif


}


