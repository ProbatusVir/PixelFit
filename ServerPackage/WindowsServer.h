#pragma once
#include <WinSock2.h>
#include <winsock.h>
#include <WS2tcpip.h>
#include <vector>
#include "CommandSet.h"
//#include <string>
#pragma comment(lib, "Ws2_32.lib")
class WindowsServer
{
public:

	WindowsServer();
	~WindowsServer();
	void Start();
	void Cleanup();
	const bool IPSetupComplete();
	

private:

	void AcquireIpAdress();
	void HandleClient(const SOCKET clientSocket);
	char _ipAddress[INET_ADDRSTRLEN] = {0};
	bool _keepAlive = true;
	CommandSet _commands;
	SOCKET serverFd;
	std::vector<SOCKET> _clients;
	// When the user files are added




};

