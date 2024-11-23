#pragma once
#include <WinSock2.h>
#include <winsock.h>
#include <WS2tcpip.h>
#include <vector>
#include "CommandSet.h"
#include "WindowsUserPair.h"
#include "WindowsInterpreter.h"
//#include <string>
#pragma comment(lib, "Ws2_32.lib")
class WindowsServer
{
public:

	WindowsServer();
	WindowsServer(int setupIPType);
	~WindowsServer();
	void Start();
	void Cleanup();
	const bool IPSetupComplete();
	
	

private:

	void AcquireIpAdress();
	void HandleClient(const SOCKET clientSocket);
	void HandleNonBlocking(SOCKET &clientSocket);
	void EmptyClientBuffer(const SOCKET& clientSocket);
	
	char _ipAddress[INET_ADDRSTRLEN] = {0};
	bool _keepAlive = true;
	
	SOCKET serverFd;
	std::vector<SOCKET> _clients;
	WindowsInterpreter _interpreter;
	// This is for the transfer from command set to create our pairs needed
	User _loginUser;
	




};

