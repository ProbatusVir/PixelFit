#pragma once
#include <winsock.h>
#include "Constants.h"

/////////////////////////////////////////////////////// Parents //////////////////////////////////////////////////////////////////
struct Header
{
	static unsigned int ReadByteHeader(const SOCKET clientSocket);

	Header(const SOCKET socket);
	Header(const unsigned int _tk_size, const char* _token, const unsigned int _bf_size);
	~Header();
	
	//This is managed by the struct. Do not delete the returned pointer.
	virtual const char* Serialize() = 0;

	unsigned int token_size;
	char* token = nullptr;
	unsigned int buffer_size;
	char* serialized = nullptr;
	size_t serialized_size = -1;
};

struct Packet : public Header
{
	Packet(const SOCKET socket, unsigned int wait = 0);
	Packet(const unsigned int _tk_size, const char* _token, const unsigned int _bf_size, const char* _buffer);
	~Packet();
	//This is managed by the struct. Do not delete the returned pointer.
	const char* Serialize() override = 0;

	char* buffer = nullptr;
};

//////////////////////////////////////////////// Inbound derivatives ///////////////////////////////////////////////////////////

struct InboundHeader : public Header
{
	InboundHeader(const SOCKET socket)
		: Header(socket) {};
	InboundHeader(const unsigned int _tk_size, const char* _token, const unsigned int _bf_size)
		: Header(_tk_size, _token, _bf_size) {};
	
	//This is managed by the struct. Do not delete the returned pointer.
	const char* Serialize() override;
};

struct InboundPacket : public Packet
{
	InboundPacket(const SOCKET socket, unsigned int wait = 0)
		: Packet(socket, wait) {};
	InboundPacket(const unsigned int _tk_size, const char* _token, const unsigned int _bf_size, const char* _buffer) 
		: Packet(_tk_size, _token, _bf_size, _buffer) {};
	
	//This is managed by the struct. Do not delete the returned pointer.
	const char* Serialize() override;
};

//////////////////////////////////////////////// Outbound derivatives ///////////////////////////////////////////////////////////

struct OutboundHeader : public InboundHeader
{
	OutboundHeader(Command command, const unsigned int _tk_size, const char* _token, const unsigned int _bf_size);
	//This is managed by the struct. Do not delete the returned pointer.
	const char* Serialize() override;

	Command command;
};

struct OutboundPacket : public InboundPacket
{
	OutboundPacket(Command _command, const unsigned int _tk_size, const char* _token, const unsigned int _bf_size, const char* buffer);
	//This is managed by the struct. Do not delete the returned pointer.
	const char* Serialize() override;

	Command command;
};
