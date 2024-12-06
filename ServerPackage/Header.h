#pragma once
#include <winsock.h>
struct Header
{
	Header(const SOCKET socket, unsigned int wait = 0);
	~Header();
	static unsigned int ReadByteHeader(const SOCKET clientSocket);
	unsigned int token_size;
	char* token;
	unsigned int buffer_size;
	char* buffer;
	
};

