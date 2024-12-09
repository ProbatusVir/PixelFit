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

Header::Header(const Command _cmd, const unsigned int _tk_size, const char* _token, const unsigned int _bf_size, const char* _buffer) :
	token_size(_tk_size),
	buffer_size(_bf_size)
{
	command = new Command(_cmd);
	token = new char[token_size + 1]; token[token_size] = '\0';
	buffer = new char[buffer_size + 1]; buffer[buffer_size] = '\0';
	memcpy_s(token,  token_size,  _token,  token_size);
	memcpy_s(buffer, buffer_size, _buffer, buffer_size);

}



Header::~Header()
{
	if (command)
		delete command;
	if (token)
		delete[] token;
	if (buffer)
		delete[] buffer;
	if (serialized)
		delete[] serialized;
}

const char* Header::Serialize()
{
	if (serialized) //This might not be best practice, but I'd rather avoid bikeshedding.
		delete[] serialized;
	
	serialized_size = sizeof(command) + sizeof(token_size) + token_size + sizeof(buffer_size) + buffer_size;
	serialized = new char[serialized_size];
	char* write_point;
	memcpy_s(write_point = serialized,				sizeof(*command),		command,		sizeof(*command));
	memcpy_s(write_point += sizeof(*command),		sizeof(token_size),		&token_size,	sizeof(token_size));
	memcpy_s(write_point += sizeof(token_size),		token_size,				token,			token_size);
	memcpy_s(write_point += token_size,				sizeof(buffer_size),	&buffer_size,	sizeof(buffer_size));
	memcpy_s(write_point += sizeof(buffer_size),	buffer_size,			&buffer,		buffer_size);
	return serialized;
}