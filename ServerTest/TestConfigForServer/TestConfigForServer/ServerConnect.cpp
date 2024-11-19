#include "ServerConnect.h"
#include <thread>
#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>
#include <direct.h>
#include <functional>
#include <filesystem>
#include <iostream>


ServerConnect::ServerConnect()
{
	SetTargetIp();
	CreateSocket();

}

ServerConnect::~ServerConnect()
{
	WSACleanup();
}

int ServerConnect::SendToServer(int command, char* message)
{
	int codeFromServer = 0;
	unsigned int tokenSize = 0;
	if (_token != nullptr)
		tokenSize = hashSize + 1;
	
	constexpr unsigned int lengthOfCommandAndMessageHeader = 9;
	const int lengthOfMessage = (int)strlen(message) + 1 + lengthOfCommandAndMessageHeader;
	
	char messageToServer[1024] = { 0 };
	

	// writes command
	memcpy_s(messageToServer, sizeOfInt, &command, sizeof(command));
	if (_token[0] != 0) {
		// creates the size of the token
		memcpy_s(messageToServer + sizeOfInt, sizeOfInt, &tokenSize, sizeOfInt);
		// writes the token
		memcpy_s(messageToServer + sizeOfInt * 2, tokenSize, _token, tokenSize);
		// writes length of message
		memcpy_s(messageToServer + sizeOfInt * 2 + tokenSize, sizeOfInt, &lengthOfMessage, sizeOfInt);
		// writes the message
		memcpy_s(messageToServer + sizeOfInt * 3 + tokenSize, lengthOfMessage, message, lengthOfMessage);
	}
	else {
		memcpy_s(messageToServer + sizeOfInt, lengthOfMessage, &lengthOfMessage, sizeof(int));
		memcpy_s(messageToServer + sizeOfInt * 2, lengthOfMessage, message, lengthOfMessage);

	}

	const unsigned int packetSize = tokenSize + lengthOfMessage;
	std::cout << '\n' << messageToServer << '\n';
	send(_client, messageToServer, packetSize , 0);

	char response[4] = { 0 };


	const int readBufferSize = recv(_client, response, 4, 0);

	if (readBufferSize > 0) {

		memcpy_s(&codeFromServer, sizeof(int), response, sizeof(int));
		if (codeFromServer > 0 && codeFromServer < 2) {
			recv(_client, response, sizeof(int), 0);
			int tokenSize = 0;
			memcpy_s(&tokenSize, sizeof(int), response, sizeof(int));
			
			
			recv(_client, _token, tokenSize, 0);
			std::cout << _token << '\n';
		

		}
		else {
			char* recvMessage = nullptr;
			unsigned int byteHeaderSize = 0;
			recv(_client, (char*)&byteHeaderSize, sizeOfInt, 0);			
			recvMessage = new char[byteHeaderSize + 1];

			recv(_client, recvMessage, byteHeaderSize, 0);
			std::cout << recvMessage << '\n';
			if (recvMessage != nullptr) delete[] recvMessage;
		}

	}
	

	return codeFromServer;

}

void ServerConnect::CreateSocket()
{
	WSADATA wsaData;
	SOCKET socketFd = INVALID_SOCKET;
	struct sockaddr_in serverAddr;
	char buffer[1024] = { 0 };

	// Initialize winsock

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "Wsa Startup failed \n";
		return;
	}

	// Create Socket

	socketFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socketFd == INVALID_SOCKET) {
		std::cerr << "Socket creation failed\n";
		WSACleanup();
		return;

	}

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(5930);

	// Convert IPV4 and IPV6 to binary

	if (inet_pton(AF_INET, _ipAddress, &serverAddr.sin_addr) <= 0) {
		std::cerr << "Error with conversion \n";
		closesocket(socketFd);
		WSACleanup();
		return;
	}
	std::cout << "Protocol set \n";
	// connect to server

	if (connect(socketFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		std::cerr << "Connection error \n";
		const int errorCode = WSAGetLastError();

		switch (errorCode)
		{
			case (WSAECONNREFUSED): std::cerr << "Connection refused. Ensure the server is running and accessible.\n"; break;
			case (WSAETIMEDOUT): std::cerr << "Connection timed out. The server might be down or not responding.\n"; break;
			case (WSAEHOSTUNREACH): std::cerr << "No route to host. Check your network connection.\n"; break;
			case (WSAENETUNREACH): std::cerr << "Network is unreachable.\n"; break;
		}

		closesocket(socketFd);
		WSACleanup();
		return;


	}

	_client = socketFd;
}

void ServerConnect::SetTargetIp()
{

	WSAData wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "Startup failed\n";

		return;
	}
	char hostName[256];
	if (gethostname(hostName, sizeof(hostName)) == SOCKET_ERROR) {
		std::cerr << "Error getting hostname.\n";
		WSACleanup();
		return;
	}

	addrinfo hints = {};
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	addrinfo* result = nullptr;

	if (getaddrinfo(hostName, nullptr, &hints, &result) != 0) {
		std::cerr << "Error getting IP address from hostname \n";
		WSACleanup();
		return;
	}


	for (addrinfo* ptr = result; ptr != nullptr; ptr = ptr->ai_next) {
		sockaddr_in* sockaddr_ipv4 = reinterpret_cast<sockaddr_in*>(ptr->ai_addr);
		if (inet_ntop(hints.ai_family, &sockaddr_ipv4->sin_addr, _ipAddress, INET_ADDRSTRLEN)) {
			break;
		}

	}


	std::cout << "Your Local IP Address is " << _ipAddress << '\n';

	freeaddrinfo(result);

	WSACleanup();
}
