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

int ServerConnect::SendToServer(Command command, char* message)
{
	int codeFromServer = 0;

	int lengthOfMessage = (int) strlen(message);
	lengthOfMessage++;
	char* messageToServer = new char[lengthOfMessage + 9];

	*messageToServer = (int) command;

	messageToServer[4] = lengthOfMessage;

	for (int i = 8; i < lengthOfMessage + 8; i++) {
		messageToServer[i] = message[i - 8];

	}

	messageToServer[lengthOfMessage + 8] = '\0';
	send(_client, messageToServer, lengthOfMessage + 9, 0);

	char response[37] = { 0 };


	int readBuffer = recv(_client, response, 4, 0);

	if (readBuffer > 0) {
		codeFromServer = (unsigned int)response[0];

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

	// connect to server

	if (connect(socketFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		std::cerr << "Connection error \n";
		int errorCode = WSAGetLastError();
		if (errorCode == WSAECONNREFUSED) {
			std::cerr << "Connection refused. Ensure the server is running and accessible.\n";
		}
		else if (errorCode == WSAETIMEDOUT) {
			std::cerr << "Connection timed out. The server might be down or not responding.\n";
		}
		else if (errorCode == WSAEHOSTUNREACH) {
			std::cerr << "No route to host. Check your network connection.\n";
		}
		else if (errorCode == WSAENETUNREACH) {
			std::cerr << "Network is unreachable.\n";
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
