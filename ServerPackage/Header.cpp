#include "Header.h"
#include <thread>
#include <chrono>

char* DelayRead(const SOCKET socket, const unsigned int wait, const size_t buffer_size) {
	char* const buffer = new char[buffer_size + 1];

	std::this_thread::sleep_for(std::chrono::seconds(wait));
	recv(socket, buffer, (int)buffer_size, 0);
	return buffer;
}

unsigned int ReadByteHeader(const SOCKET clientSocket)
{
	unsigned int byteHeader = 0;
	recv(clientSocket, (char*)&byteHeader, sizeof(unsigned int), 0);
	return byteHeader;
}

Header::Header(const SOCKET socket)
{
	token_size = ReadByteHeader(socket);
	
	if (token_size)
	{
		token = new char[token_size + 1];
		recv(socket, token, token_size, 0);
	}

	buffer_size = ReadByteHeader(socket);
}

Header::Header(const unsigned int _tk_size, const char* _token, const unsigned int _bf_size) :
	token_size(_tk_size),
	buffer_size(_bf_size)
{
	token = new char[token_size + 1]; token[token_size] = '\0';
	memcpy_s(token, token_size, _token, token_size);
}

Header::~Header()
{
	if (token)
		delete[] token;
	if (serialized)
		delete[] serialized;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Packet::Packet(const SOCKET socket, unsigned int wait) :
	Header(socket)
{
	if (buffer_size)
		buffer = DelayRead(socket, wait, buffer_size);
}

Packet::Packet(const unsigned int _tk_size, const char* _token, const unsigned int _bf_size, const char* _buffer) :
	Header(_tk_size, _token, _bf_size)
{
	buffer = new char[buffer_size + 1]; buffer[buffer_size] = '\0';
	memcpy_s(buffer, buffer_size, _buffer, buffer_size);
}



Packet::~Packet()
{
	if(buffer)
		delete[] buffer;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

OutboundHeader::OutboundHeader(Command _command, const unsigned int _tk_size, const char* _token, const unsigned int _bf_size) :
	InboundHeader(_tk_size, _token, _bf_size)
{
	command = _command;
}

const char* OutboundHeader::Serialize()
{
	//commmand ts, t, bs, buffer			
	const char* data[] = { (const char*)&command, (const char*)&token_size, (const char*)token, (const char*)&buffer_size};
	const size_t sizes[] = { sizeof(command),		sizeof(token_size),		token_size,			sizeof(buffer_size)};

	return CONCATENATEB(data, sizes, serialized, serialized_size);
}

OutboundPacket::OutboundPacket(Command _command, const unsigned int _tk_size, const char* _token, const unsigned int _bf_size, const char* _buffer) :
	InboundPacket(_tk_size, _token, _bf_size, _buffer)
{
	command = _command;
}

const char* OutboundPacket::Serialize()
{
	//commmand ts, t, bs, buffer			
	const char* data[] = { (const char*)&command, (const char*)&token_size, (const char*)token, (const char*)&buffer_size,	buffer };
	const size_t sizes[] = { sizeof(command),		sizeof(token_size),		token_size,			sizeof(buffer_size),		buffer_size };
	
	return CONCATENATEB(data, sizes, serialized, serialized_size);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* InboundHeader::Serialize()
{
	//ts, t, bs
	const char* data[] = { (const char*)&token_size, (const char*)token, (const char*)&buffer_size };
	const size_t sizes[] = { sizeof(token_size),		token_size,			sizeof(buffer_size) };
	
	return CONCATENATEB(data, sizes, serialized, serialized_size);
}

const char* InboundPacket::Serialize()
{
	//ts, t, bs, buffer			
	const char* data[] = { (const char*)&token_size, (const char*)token, (const char*)&buffer_size, buffer };
	const size_t sizes[] = { sizeof(token_size),		token_size,			sizeof(buffer_size),	buffer_size };

	return CONCATENATEB(data, sizes, serialized, serialized_size);
}

/// <summary>
/// Concatenate an arbitrary number of strings, where all sizes are known, and you wish bind the array to a particular address, and wish to capture the array size
/// </summary>
/// <param name="fields">The number of items in the data and sizes arrays</param>
/// <param name="data">An array of data</param>
/// <param name="sizes:">Array of sizes corresponding to the data values' sizes</param>
/// <param name="arr:">The pointer variable to be reassigned to point to the new data</param>
/// <param name="arr_size:">Will be reassigned to the new array's size</param>
/// <returns></returns>
const char* Concatenator(const size_t fields, const char** data, const size_t* sizes, char*& array, size_t& array_size)
{
	if (array)
		delete[] array;
	array_size = 0;

	char* write_point;
	//Get buffer size
	for (int i = 0; i < fields; i++)
		array_size += sizes[i];

	array = new char[array_size + 1]; array[array_size] = '\0';
	write_point = array;

	//Fill buffer
	for (int i = 0; i < fields; i++)
	{
		memcpy(write_point, data[i], sizes[i]);
		write_point += sizes[i];
	}

	return array;
}
/// <summary>
/// Concatenate an arbitrary number of fields where all strings are null terminated
/// </summary>
/// <param name="fields"></param>
/// <param name="data"></param>
/// <returns></returns>
const char* Concatenator(const size_t fields, const char** data)
{
	size_t array_size = 0;
	char* array;
	char* write_point;
	size_t* sizes = new size_t[fields];

	//Get buffer size
	for (int i = 0; i < fields; i++)
		array_size += sizes[i] = strlen(data[i]);

	array = new char[array_size + 1]; array[array_size] = '\0';
	write_point = array;

	//Fill buffer
	for (int i = 0; i < fields; i++)
	{
		memcpy(write_point, data[i], sizes[i]);
		write_point += sizes[i];
	}

	return array;
}

/// <summary>
/// Concatenate an arbitrary number of fields where all sizes are known
/// </summary>
/// <param name="fields"></param>
/// <param name="data"></param>
/// <param name="sizes"></param>
/// <returns></returns>
const char* Concatenator(const size_t fields, const char** data, const size_t* sizes)
{
	size_t array_size = 0;
	char* array;
	char* write_point;
	//Get buffer size
	for (int i = 0; i < fields; i++)
		array_size += sizes[i];

	array = new char[array_size + 1]; array[array_size] = '\0'; //just in case...
	write_point = array;

	//Fill buffer
	for (int i = 0; i < fields; i++)
	{
		memcpy(write_point, data[i], sizes[i]);
		write_point += sizes[i];
	}

	return array;
}

/////////////////////////////////////////////////////////////////////////////////////

InboundResourcePacket::InboundResourcePacket(const SOCKET socket, unsigned int wait) :
	InboundResourceHeader(socket)
{
	if (buffer_size)
		buffer = DelayRead(socket, wait, buffer_size);
}

InboundResourceHeader::InboundResourceHeader(const SOCKET socket) :
	InboundHeader(socket)
{
	type = (ResourceType)ReadByteHeader(socket);
	buffer_size -= sizeof(ResourceType);
}
