#include "Header.h"
#include <thread>
#include <chrono>
unsigned int Header::ReadByteHeader(const SOCKET clientSocket)
{
	unsigned int byteHeader = 0;
	recv(clientSocket, (char*)&byteHeader, sizeof(unsigned int), 0);
	return byteHeader;
}

Header::Header(const SOCKET socket, unsigned int wait)
{
	token_size = ReadByteHeader(socket);
	token = new char[token_size + 1];
	recv(socket, (char*)token, token_size, 0);
	
	buffer_size = ReadByteHeader(socket);
	buffer = new char[buffer_size + 1];

	//This is awful... please don't keep this...
	std::this_thread::sleep_for(std::chrono::seconds(wait));
	recv(socket, buffer, buffer_size, 0);
}



Header::~Header()
{
	if (token)
		delete[] token;
	if (buffer)
		delete[] buffer;
}
