#pragma once

#include <WS2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

class ServerConnect
{
public: 
	ServerConnect(int lanOrLocalhost);

	~ServerConnect();

	int SendToServer(int command, const char* message);

private:
	void CreateSocket();
	void ListenForServer();
	void SetTargetIp();
	void HandleToken();
	unsigned int ReadHeader() const;
	void ReadMessageFromServer() const;

	char _ipAddress[INET_ADDRSTRLEN] = { 0 };

	SOCKET _client;
	// length of secure token
	char _token[33] = { 0 };


};

