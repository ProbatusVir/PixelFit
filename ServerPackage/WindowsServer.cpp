#include "WindowsServer.h"
#include <iostream>
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
#include "Constants.h"


// use port numbers from 3500 - 65000. 
// Avoid using ports lower than 3500 due to IATA protocols

#define PORT 5930

WindowsServer::WindowsServer()
{
	AcquireIpAdress();

	WSAData wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "Startup failed \n";
		return;
	}
	serverFd = socket(AF_INET, SOCK_STREAM, 0);
	_commands = CommandSet();
	
}

WindowsServer::~WindowsServer()
{
	WSACleanup();
}

void WindowsServer::Start()
{
	struct sockaddr_in address = {};
	int addrSize = sizeof(address);

	if (serverFd == INVALID_SOCKET) {
		std::cerr << "Socket creation failed\n";
		WSACleanup();
		return;
	}

	address.sin_family = AF_INET;
	address.sin_port = htons(PORT);
	inet_pton(AF_INET, _ipAddress, &address.sin_addr);

	if (bind(serverFd, (sockaddr*)&address, addrSize) == SOCKET_ERROR) {
		std::cerr << "Error binding the socket\n";
		WSACleanup();
		return;
	}
	else std::cout << "Socket bind success\n";

	if (listen(serverFd, 3) == SOCKET_ERROR) {
		std::cerr << "Error on listen\n";
		WSACleanup();
		return;
	}


	fd_set readFds;
	timeval timeout;
	timeout.tv_sec = 5;

	// Handle thread for graceful shutdown

	while (_keepAlive) {

		FD_ZERO(&readFds);
		FD_SET(serverFd, &readFds);

		int activity = select(0, &readFds, NULL, NULL, &timeout);

		if (activity != 0) {
			SOCKET client;

			sockaddr_in clientAddr = {};

			int clientAddrSize = sizeof(clientAddr);

			client = accept(serverFd, (sockaddr*)&clientAddr, &clientAddrSize);
			std::cout << "Client accepted \n";
			if (client == INVALID_SOCKET) {
				std::cerr << "Socket accept failed\n";
				closesocket(client);
				continue;
			}

			_clients.push_back(client);

			auto boundClient = std::bind(&WindowsServer::HandleClient, this, client);
			std::thread clientThread(boundClient);
			clientThread.detach();


		}

		// This is to check if clients sent anything to the server.
		auto clientIter = _clients.begin();

		for (; clientIter != _clients.end(); clientIter++) {
			SOCKET clientSocket = *clientIter;
			if (FD_ISSET(clientSocket, &readFds)) {

				auto boundClient = std::bind(&WindowsServer::HandleClient, this, clientSocket);

				std::thread activeClient(boundClient);
				activeClient.detach();
			}
		}


	}
}

void WindowsServer::Cleanup()
{
	// Call write methods to save the data on the server

	// Clean up any dynamic memory

}

bool WindowsServer::IPSetupComplete()
{
	if (_ipAddress[0] || _ipAddress[1] != 0) {
		return true;
	}
	return false;
}

void WindowsServer::HandleClient(SOCKET clientSocket)
{


	char buffer[1024] = { 0 };

	int readBuffer = recv(clientSocket, buffer, 4, 0);
	const int command = (int)buffer[0];
	if (readBuffer >= 0) {
		unsigned int amountToRead = 0;
		//recv(clientSocket, (char*)amountToRead, 4, 0);
		recv(clientSocket, buffer, 1023, 0);
		
		_commands.InterpretRequest((Command)command, buffer);
		char response[4] = { 1 };
		send(clientSocket, (char*)&response, 4, 0);

	}
	else if (readBuffer == 0 || readBuffer == SOCKET_ERROR) {
		std::cout << "Client disconnected \n";
		closesocket(clientSocket);

		for (auto clientIter = _clients.begin(); clientIter != _clients.end();) {
			if (*clientIter == clientSocket)
				clientIter = _clients.erase(clientIter);
			else clientIter++;
		}

	}
}

void WindowsServer::AcquireIpAdress()
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
