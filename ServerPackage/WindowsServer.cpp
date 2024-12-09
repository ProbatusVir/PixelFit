#include "WindowsServer.h"
#include "Constants.h"

#include <iostream>
#include <thread>

void ulongtoaddr(unsigned long n, char ip[16]);

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

WindowsServer::WindowsServer(int setupIPType)
{
	if (setupIPType == 1)
		AcquireIpAdress();
	else
		memcpy_s(_ipAddress, sizeof(_ipAddress), localhost, sizeof(localhost));
	
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
	std::cout << "Binding to " << _ipAddress << ':' << port << '\n';

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
//	std::thread monitorClients(&WindowsServer::MonitorClients, this);

	/*
		This loop will continue to accept clients and process clients.
		the activity variable checks to see if anything has happened on the server.
		maxFds is getting a reference to the traffic to the server.

	*/
	while (_keepAlive) {

		FD_ZERO(&readFds);
		FD_SET(serverFd, &readFds);

		SOCKET maxFd = serverFd;

		for (const auto& client : _clients) {
			FD_SET(client, &readFds);
			if (client > maxFd) {
				maxFd = client;
			}
		}

		int activity = select((int)(maxFd + 1), &readFds, NULL, NULL, &timeout);

	
		// Checking activity on the server
		if (activity != 0) {
			bool activityFromConnectedClient = false;
			auto clientIter = _clients.begin();
			// Checks clients for activity
			for (; clientIter != _clients.end();) {
				if (FD_ISSET(*clientIter, &readFds)) {

					char testData[4];
					int bytesRead = recv(*clientIter, testData, sizeOfInt, MSG_PEEK);
					if (bytesRead > 0 && testData[0] != 0) {
						HandleClient(*clientIter);
						// activity was a connected client, not needing a new accept call
						activityFromConnectedClient = true;
						EmptyClientBuffer(*clientIter);
					}
					else if (bytesRead == 0) {
						std::cout << "Client disconnected\n";
						_interpreter.DisconnectClient(*clientIter);
						clientIter = _clients.erase(clientIter);
						activityFromConnectedClient = true;
						continue;
					}
					else if (bytesRead == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK) {
						std::cerr << "Socket error: " << WSAGetLastError() << "\n";
						_interpreter.DisconnectClient(*clientIter);
						clientIter = _clients.erase(clientIter); 
						activityFromConnectedClient = true;
						continue;
					}
				}
				clientIter++;
			}
			// Prevents clients from being added a second time to the server
			if (!activityFromConnectedClient) 
			{

				SOCKET client;

				sockaddr_in clientAddr = {};

				int clientAddrSize = sizeof(clientAddr);

				char ip[16];
				// Accepts clients to the server
				client = accept(serverFd, (sockaddr*)&clientAddr, &clientAddrSize);
				ulongtoaddr(clientAddr.sin_addr.S_un.S_addr, ip);
				std::cout << "Client accepted.\tIP: " << ip << '\n';
				if (client == INVALID_SOCKET) {
					std::cerr << "Socket accept failed\n";
					closesocket(client);
					continue;
				}
				// Adjust the client socket to be non-blocking, this allows us to know if a client has sent garbage data
				HandleNonBlocking(client);
				// Adds clients to the connected vector
				_clients.push_back(client); 
			}
		}
	}
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


	char readCmd[sizeof(Command)];
	int readBuffer = recv(clientSocket, readCmd, sizeof(Command), MSG_PEEK);

	if (readBuffer > 0) {
		recv(clientSocket, readCmd, sizeof(Command), 0); // Consume the bytes
		Command command = static_cast<Command>(readCmd[0]);
		_interpreter.InterpretMessage(clientSocket, command);
	}
	else if (readBuffer == 0) {
		std::cout << "Client disconnected\n";
		_interpreter.DisconnectClient(clientSocket);
	}
	else if (readBuffer == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK) {
		std::cerr << "Socket error: " << WSAGetLastError() << "\n";
		_interpreter.DisconnectClient(clientSocket);
	}

}
// Handles the login of each client to pair them to a user object and their respective socket
// Following this process, our CheckClient function will be able to contact the correct clients.

void WindowsServer::HandleNonBlocking(SOCKET& clientSocket)
{
	// changes to non-blocking
	u_long mode = 1; // 1 to enable non-blocking
	if (ioctlsocket(clientSocket, FIONBIO, &mode) != 0) {
		std::cerr << "Failed to set non-blocking mode: " << WSAGetLastError() << std::endl;
		closesocket(clientSocket);
	}


}
// This prevents the lockups if a client sends to much data.
// This is absolutely required otherwise unexpected behavior happens.
void WindowsServer::EmptyClientBuffer(const SOCKET& clientSocket)
{
	char throwAway[1];
	int bytesRead = 0;
	while (bytesRead > -1) {
		bytesRead = recv(clientSocket, throwAway, 1, MSG_PEEK);
		if (bytesRead > 0) {
			bytesRead = recv(clientSocket, throwAway, 1, 0);
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
		std::cerr << "Error getting IP address from hostname\n";
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

//The IP will need to be 16 bytes. 12 for the numbers, 3 for the dots, and 1 for the terminator
void ulongtoaddr(unsigned long _n, char* ip) {
	//this is pretty easy to make type-agnostic as a macro
	unsigned char* n = (unsigned char*)&_n;
	char seeker = 0;
	
	for (int i = 0; i < sizeof(_n); i++, seeker++)
	{
		const unsigned char digits = (n[i] ? floor(log10(n[i])) : (unsigned char)0) + 1;
		n[i];
		_itoa_s(n[i], ip + seeker, digits + 1, 10);
		seeker += digits;
		ip[seeker] = '.';
	}

	ip[seeker - 1] = '\0';
}