#pragma once
#include <winsock.h>
struct Header
{
	Header(const SOCKET socket);
	~Header();
	static unsigned int ReadByteHeader(const SOCKET clientSocket);
	unsigned int token_size;
	unsigned char* token;
	unsigned int buffer_size;
	char* buffer;
	
};

