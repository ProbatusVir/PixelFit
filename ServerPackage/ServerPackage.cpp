// ServerPackage.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include "WindowsServer.h"

int main()
{
#if defined(_WIN32)

	WindowsServer server;
	if (server.IPSetupComplete()) {
		server.Start();
	}

#elif defined(_linux_)
	std::cout << "Linux operating system \n"


#endif


}


