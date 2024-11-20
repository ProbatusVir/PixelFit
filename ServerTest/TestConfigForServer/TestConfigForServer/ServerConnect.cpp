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
	
	
	constexpr unsigned int lengthOfCommandAndMessageHeader = sizeOfInt + sizeOfInt + sizeof(tokenSize) + 1;
	const int lengthOfMessage = (int)strlen(message) + lengthOfCommandAndMessageHeader;
	
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
	// This is creating the entire packet size to send
	const unsigned int packetSize = tokenSize + lengthOfMessage;
	std::cout << '\n' << messageToServer << '\n';
	send(_client, messageToServer, packetSize , 0);

	char response[4] = { 0 };



	

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
	std::thread serverListen(&ServerConnect::ListenForServer, this);
	serverListen.detach();
}

void ServerConnect::ListenForServer()
{
	fd_set notificationFromServer;
	while (true) {
		FD_ZERO(&notificationFromServer);
		FD_SET(_client, &notificationFromServer);


		int activity = select(_client + 1, &notificationFromServer, NULL, NULL, NULL);

		if (activity < 0) {
			std::cout << "Error in listening loop\n";
		}

		if (FD_ISSET(_client, &notificationFromServer)) {
			char command[4] = { 0 };
			int bytesRead = recv(_client, command, 4, 0);
			if (bytesRead > 0) {
				unsigned int cmd = 0;
				memcpy_s(&cmd, sizeOfInt, command, sizeOfInt);
				switch (cmd) {
				case 1:
					HandleToken();
					break;
				case 2:
					ReadMessageFromServer();
					break;
				case (int) Command::NewDiscussionPost:
					ReadMessageFromServer();
					break;
				}
				
				// Handle commands as necessary
			}
			else if (bytesRead == 0 || bytesRead == SOCKET_ERROR) {
				std::cout << "Server Disconnected\n";
					break;
			}
			else {
				std::cout << "Recv failed breaking connection\n";
				closesocket(_client);
				break;
			}
		}
	}
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

void ServerConnect::HandleToken()
{
	unsigned int bytesToRead = ReadHeader();
	if (bytesToRead > 0) {
		char* token = new char[hashSize + 1];
		recv(_client, token, bytesToRead, 0);

		memcpy_s(_token, hashSize + 1, token, bytesToRead);

		std::cout << _token << '\n';
	}
}

unsigned int ServerConnect::ReadHeader()
{
	char buffer[4] = { 0 };
	recv(_client, buffer, 4, 0);
	unsigned int header = 0;
	memcpy_s(&header, sizeOfInt, buffer, sizeOfInt);

	return header;
}

void ServerConnect::ReadMessageFromServer()
{
	unsigned int byteHeader = ReadHeader();
	if (byteHeader) {
		char* message = new char[byteHeader + 1];

		recv(_client, message, byteHeader, 0);
		std::cout << message << '\n';

		delete[] message;
		char sendBack[4] = { 0 };
		*sendBack = 1;
		send(_client, sendBack, sizeOfInt, 0);
	}
}
