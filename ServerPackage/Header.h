#pragma once
#include <winsock.h>
#include "Constants.h"

struct Header
{
	Header(const SOCKET socket, unsigned int wait = 0);
	Header(const Command _cmd, const unsigned int _tk_size, const char* _token, const unsigned int _bf_size, const char* _buffer);
	~Header();
	//This is managed by the struct. Do not delete the returned pointer.
	const char* Serialize();
	static unsigned int ReadByteHeader(const SOCKET clientSocket);

	Command* command = nullptr; //I don't love this, but command should be nullable.
	unsigned int token_size;
	char* token = nullptr;
	unsigned int buffer_size;
	char* buffer = nullptr;
	char* serialized = nullptr;
	int serialized_size = -1;
};

