#pragma once

#include <WinSock2.h>
#include <winsock.h>
#include <WS2tcpip.h>
#include <vector>
#include "../../../ServerPackage/Constants.h"
#pragma comment(lib, "Ws2_32.lib")

class ServerConnect
{
public: 
	ServerConnect(int lanOrLocalhost);

	~ServerConnect();

	int SendToServer(int command, char* message);

private:
	void CreateSocket();
	void ListenForServer();
	void SetTargetIp();
	void HandleToken();
	unsigned int ReadHeader();
	void ReadMessageFromServer();

	char _ipAddress[INET_ADDRSTRLEN] = { 0 };

	SOCKET _client;
	// length of secure token
	char _token[33] = { 0 };


};

