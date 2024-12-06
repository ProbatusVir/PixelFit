#include "Header.h"

unsigned int Header::ReadByteHeader(const SOCKET clientSocket)
{
	unsigned int byteHeader = 0;
	recv(clientSocket, (char*)&byteHeader, sizeof(unsigned int), 0);
	return byteHeader;
}

Header::Header(const SOCKET socket)
{
	token_size = ReadByteHeader(socket);
	token = new unsigned char[token_size + 1];
	recv(socket, (char*)token, token_size, 0);
	
	buffer_size = ReadByteHeader(socket);
	buffer = new char[buffer_size + 1];
	recv(socket, buffer, buffer_size, 0);
}

Header::~Header()
{
	if (token)
		delete[] token;
	if (buffer)
		delete[] buffer;
}
