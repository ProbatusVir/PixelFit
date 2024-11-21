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

WindowsServer::WindowsServer()
{
	AcquireIpAdress();

	WSAData wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "Startup failed \n";
		return;
	}
	serverFd = socket(AF_INET, SOCK_STREAM, 0);
	_interpreter = WindowsInterpreter();

}

WindowsServer::~WindowsServer()
{
	Cleanup();
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
	address.sin_port = htons(port);
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
	std::thread monitorClients(&WindowsServer::MonitorClients, this);


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



		}



	}
	
	monitorClients.join();
}

void WindowsServer::Cleanup()
{
	// Call methods to send data to sql server

	// Clean up any dynamic memory

	for (SOCKET client : _clients) {
		closesocket(client);

	}
	WSACleanup();
}

const bool WindowsServer::IPSetupComplete()
{
	return (_ipAddress[0] && _ipAddress[1] != 0);
}

void WindowsServer::HandleClient(const SOCKET clientSocket)
{

	// TODO: Make this look better (rs)
	Command command;
	unsigned int amountToRead = 0;
	char readCmd[sizeof(Command)];
	char bytesToRead[5] = { 0 };
	char response[4] = { 0 };

	//Receive file header

	const int readBuffer = recv(clientSocket, readCmd, sizeOfInt, 0);
	command = static_cast<Command>(readCmd[0]);



	if (readBuffer > 0) {
	
		_interpreter.InterpretMessage(clientSocket, command);
	}
	else if (readBuffer == 0 || readBuffer == SOCKET_ERROR) {
		std::cout << "Client disconnected \n";
		
		
		auto client = _clients.begin();
		for (; client != _clients.end();) {
			if (*client == clientSocket) {
				client = _clients.erase(client);
			}
			else client++;
		}
		_interpreter.DisconnectClient(clientSocket);
	}
	
}
// Handles the login of each client to pair them to a user object and their respective socket
// Following this process, our CheckClient function will be able to contact the correct clients.
void WindowsServer::MonitorClients()
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


		
				auto client = clientTemp.begin();
				for (; client != clientTemp.end();client++) {
					if (FD_ISSET(*client, &clientFds)) {
						HandleClient(*client);
					}
					
				}
			

		}


	}

}
// This checks on every logged in client
// If there are incoming requests from logged in clients
// this will handle their requests. This also allows us to push to clients.






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
