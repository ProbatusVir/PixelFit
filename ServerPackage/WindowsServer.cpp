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

	// Begins monitoring clients
	std::thread monitorClients(&WindowsServer::CheckClients, this);
	
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

			auto boundClient = std::bind(&WindowsServer::HandleClient, this, client);
			std::thread clientThread(boundClient);
			clientThread.detach();
			_clients.push_back(client);



		}



	}
	monitorClients.join();
}

void WindowsServer::Cleanup()
{
	// Call write methods to save the data on the server

	// Clean up any dynamic memory

}

const bool WindowsServer::IPSetupComplete()
{
	return (_ipAddress[0] && _ipAddress[1] != 0);
}

void WindowsServer::HandleClient(const SOCKET clientSocket)
{
	Command command;
	unsigned int amountToRead = 0;
	char readCmd[sizeof(Command)];
	char bytesToRead[5] = { 0 };
	//Receive file header
	const int readBuffer = recv(clientSocket, readCmd, sizeof(int), 0);
	command = static_cast<Command>(readCmd[0]);
	recv(clientSocket, bytesToRead, sizeof(int), 0);
	memcpy_s(&amountToRead, 4, bytesToRead, 4);
	char* buffer = new char[amountToRead];


	if (readBuffer > 0) {
		recv(clientSocket, buffer, amountToRead, 0);

		_commands.InterpretRequest(command, buffer);
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
	delete[] buffer;
}

void WindowsServer::CheckClients()
{
	fd_set clientFds;

	while (_keepAlive) {
		FD_ZERO(&clientFds);
		std::vector<SOCKET> clientTemp = _clients;

		for (auto client : clientTemp) {
			FD_SET(client, &clientFds);
		}

		if (_clients.size() > 0) {

			int maxFds = static_cast<int>(*std::max_element(_clients.begin(), _clients.end())) + 1;

			int activity = select(maxFds, &clientFds, NULL, NULL, NULL);

			for (auto client : clientTemp) {
				if (FD_ISSET(client, &clientFds) ) {
					HandleClient(client);
				}
			}
		}

		//for (int i = 0; i < clientTemp.size(); i++) {
		//	const SOCKET temp = clientTemp[i];
		//	FD_SET(temp, &clientFds);
		//	int activity = select(5, &clientFds, NULL, NULL, NULL);

		//	if (FD_ISSET(temp, &clientFds) && activity != 0) {
		//		auto boundClient = std::bind(&WindowsServer::HandleClient, this, temp);
		//		std::thread clientThread(boundClient);
		//		clientThread.detach();
		//	}
		//}

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
