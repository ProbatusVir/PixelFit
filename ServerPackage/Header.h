#pragma once
#include <winsock.h>
#include "Constants.h"

#define CONCATENATE(data) Concatenator(sizeof(data) / sizeof(data[0]), data) //Concatenate an arbitrary number of fields where all strings are null terminated
#define CONCATENATEA(data, sizes) Concatenator(sizeof(data) / sizeof(data[0]), data, sizes); static_assert(sizeof(data) / sizeof(data[0]) == sizeof(sizes) / sizeof(sizes[0])); //Concatenate an arbitrary number of fields where all sizes are known
#define CONCATENATEB(data, sizes, arr, arr_size) Concatenator(sizeof(data) / sizeof(data[0]), data, sizes, arr, arr_size); static_assert(sizeof(data) / sizeof(data[0]) == sizeof(sizes) / sizeof(sizes[0])); //Concatenate an arbitrary number of strings, where all sizes are known, and you wish bind the array to a particular address, and wish to capture the array size 
const char* Concatenator(const size_t fields, const char** data, const size_t* sizes);
const char* Concatenator(const size_t fields, const char** data, const size_t* sizes, char*& array, size_t& array_size);
const char* Concatenator(const size_t fields, const char** data);
static unsigned int ReadByteHeader(const SOCKET clientSocket);
static char* DelayRead(const SOCKET socket, const unsigned int wait, const size_t buffer_size);


/////////////////////////////////////////////////////// Parents //////////////////////////////////////////////////////////////////
struct Header
{
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct InboundResourceHeader : public InboundHeader
{
	InboundResourceHeader(const SOCKET socket);
	ResourceType type;
};

struct InboundResourcePacket : public InboundResourceHeader
{
	InboundResourcePacket(const SOCKET socket, unsigned int wait = 0);
	char* buffer = nullptr;
};
