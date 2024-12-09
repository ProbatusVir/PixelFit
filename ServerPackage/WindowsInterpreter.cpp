#include "WindowsInterpreter.h"
#include "FileOps.h"

#include <iostream>
#include <functional>
#include <thread>
#include <fstream>
#include <chrono>

void WindowsInterpreter::InterpretMessage(const SOCKET& clientSocket, Command command)
{
	switch (command) {
	case Command::Login:
		// login functions
		HandleLoginUser(clientSocket);
		break;

	case Command::NewUser:
		// new user function
		HandleNewUser(clientSocket);
		break;
	case Command::MessageServer:
		// test to send data to server.
		// this could be removed at a later time or implemented to handle p2p messaging
		MessageToServer(clientSocket);
		break;
	case Command::NewDiscussionPost:
		NewDiscussionPost(clientSocket);
		break;
	case Command::GetUsers:
		// function to return users to a client
		// this can be done based on friend list, shared groups and the sent info must be 
		// restricted to name/username, and id. 

		break;

	case Command::BanUser:
		// restrictive function only allowing certain people to ban users, likely from group, or altogether from the app.
		break;

	case Command::SendImageToServer:
		ReceiveImage(clientSocket);
		break;
	case Command::RequestData: 
		SendData(clientSocket);
		break;
	case Command::LogOut:
		LogOut(clientSocket);
		break;
	}
}

void WindowsInterpreter::DisconnectClient(const SOCKET& clientSocket)
{
	if (_clientPairs.size()) {
		auto client = _clientPairs.begin();
		for (; client != _clientPairs.end();) {
			if (client->second.clientSocket == clientSocket) {
				client = _clientPairs.erase(client);
			}
			else client++;
		}

	}

	closesocket(clientSocket);
}

void WindowsInterpreter::HandleLoginUser(const SOCKET& clientSocket)
{

	unsigned int sizeOfHeader = ReadByteHeader(clientSocket);
	if (sizeOfHeader != 0) {
		char* buffer = new char[sizeOfHeader + 1];
		unsigned int bytesRead = recv(clientSocket, buffer, sizeOfHeader, 0);
		if (bytesRead != 0) {
			bool success = true;
			User user = _commands.LoginUser(buffer, success);
			LoginResponseToUser(clientSocket, user, success);
		}
		

		delete[] buffer;
	}
	else {
		SendMessageToClient(clientSocket, false);
	}
}

void WindowsInterpreter::HandleNewUser(const SOCKET& clientSocket)
{
	unsigned int sizeOfHeader = ReadByteHeader(clientSocket);

	if (sizeOfHeader != 0) {
		char* buffer = new char[sizeOfHeader + 1];
		unsigned int bytesRead = recv(clientSocket, buffer, sizeOfHeader, 0);
		if (bytesRead != 0) {
			bool success = true;
			User newUser = _commands.NewUser(buffer, success);
			LoginResponseToUser(clientSocket, newUser, success);
		}
		
		delete[] buffer;
	}

	else SendMessageToClient(clientSocket, false);

}

void WindowsInterpreter::LoginResponseToUser(const SOCKET& clientSocket, User& user, const bool success)
{
	if (success) {

		static constexpr unsigned int sizeOfToken = HASH_SIZE + 1;
		static constexpr unsigned int sizeOfResponse = sizeOfInt * 2 + sizeOfToken;
		static constexpr unsigned int msgSuccess = (int)MessageResult::LoginSuccess;
		char response[sizeOfResponse];

		std::string tokenAsStr = CreateToken(user);
		char* token = user.Token();
		// This is necessary for our response array to be sized with null terminator
		memcpy_s(response, sizeOfInt, &msgSuccess, sizeOfInt);
		memcpy_s(response + sizeOfInt, sizeOfInt, &sizeOfToken, sizeOfInt);
		memcpy_s(response + sizeOfInt * 2, sizeOfToken, token, sizeOfToken);
		send(clientSocket, response, sizeOfResponse, 0);
		WindowsUserPair clientPair;
		clientPair.user = user;
		clientPair.clientSocket = clientSocket;
		clientPair.token = token;

		std::pair<std::string, WindowsUserPair> newPair(tokenAsStr, clientPair);
		_clientPairs.insert(newPair);

		std::cout << "Successfully logged in user \"" << user.Username() << "\"!\n";

	}
	else {
		static constexpr const char failedAttempt[] = "Unauthorized username or password";
		static constexpr const unsigned int sizeOfResponse = sizeOfInt * 2 + sizeof(failedAttempt);
		static constexpr unsigned int failed = (unsigned int)MessageResult::Failed;
		static constexpr unsigned int lengthOfFailedAttempt = sizeof(failedAttempt) - 1;

		char response[sizeOfResponse];

		memcpy_s(response, sizeOfInt, &failed, sizeOfInt);
		memcpy_s(response + sizeOfInt, sizeOfInt, &lengthOfFailedAttempt, sizeOfInt);
		memcpy_s(response + sizeOfInt * 2, lengthOfFailedAttempt, failedAttempt, lengthOfFailedAttempt);
		send(clientSocket, response, sizeOfResponse, 0);
	}
}

void WindowsInterpreter::MessageToServer(const SOCKET& clientSocket)
{
	User user;
	if (VerifyUserAuth(clientSocket, user)) {
		unsigned int sizeOfHeader = ReadByteHeader(clientSocket);
		bool success = false;
		char* buffer = new char[sizeOfHeader + 1];

		int bytesRead = recv(clientSocket, buffer, sizeOfHeader, 0);
		if (bytesRead) {
			success = true;
			std::cout << buffer << '\n';
			SendMessageToClient(clientSocket, success);
		}
		else SendMessageToClient(clientSocket, success);

	}
	else SendMessageToClient(clientSocket, false);
}

//TODO: Can probably clean this up too.
void WindowsInterpreter::SendMessageToClient(const SOCKET& clientSocket, bool success)
{
	static constexpr const char success_message[] = "Message Received";
	static constexpr const char fail_message[] = "Error reading message";
	unsigned int command;
	unsigned int message_length;
	const char* message;
	if (success) {
		message = success_message;
		command = (unsigned int)MessageResult::Success;
		message_length = sizeof(success_message);
	}
	else {
		message = fail_message;
		message_length = sizeof(fail_message);
		command = (unsigned int)MessageResult::Failed;
	}

	const unsigned int packet_size = message_length + sizeOfInt * 2;
	char response[PACKET_SIZE];
	memcpy_s(response, sizeOfInt, &command, sizeOfInt);
	memcpy_s(response + sizeOfInt, sizeOfInt, &message_length, sizeOfInt);
	memcpy_s(response + sizeOfInt * 2, message_length, message, message_length);
	send(clientSocket, response, PACKET_SIZE, 0);
}

void WindowsInterpreter::SendData(const SOCKET clientSocket)
{
	Header header(clientSocket);

	char* reader = header.buffer;
	int resource_type = 0;
	memcpy_s(&resource_type, sizeOfInt, reader, sizeOfInt);
	reader += sizeOfInt;

	switch (resource_type)
	{
	case (int)ResourceType::PNG:
		SendImage(clientSocket, header);
		break;
	}
}

//Prolly gonna send the user's token back to them at some point, just to verify this message, but for now... meh
//FIXME: make me actually work, there's some funnky stuff going on with getting the image name, and that's the only problem...
void WindowsInterpreter::SendImage(const SOCKET clientSocket, const Header& header)
{
	constexpr static const Command command = Command::RequestData;
	constexpr static const ResourceType type = ResourceType::PNG;
	constexpr static const char suffix[] = ".png";
	const char* reader = header.buffer + sizeof(int);

	char* image_name;
	FileOps fop = FileOps();

	const char* file_path = EnvironmentFile::Instance()->FetchEnvironmentVariable("file_save_path");
	const size_t file_path_size = strlen(file_path);
	const size_t name_size = strlen(reader);

	image_name = new char[file_path_size + name_size + sizeof(suffix)];
	memcpy_s(image_name,								file_path_size, file_path,	file_path_size);
	memcpy_s(image_name + file_path_size,				name_size,		reader,		name_size);
	memcpy_s(image_name + file_path_size +	name_size,	sizeof(suffix), suffix,		sizeof(suffix));


	const char* data = fop.ReadFullFile(image_name, false);
	const size_t file_size = fop.FileSize();
	
	//Create message

	//FIXME: Find out if this can be sent as two messages rather than reinitializing this HUGE array just to add 4 bytes
	const unsigned int message_size = (unsigned int)(sizeof(type) + file_size); //no null
	//char* message = new char[message_size];
	//char* writer = message;
	//
	//const unsigned int& token_size = header.token_size;
	//const char* token = header.token;
	//
	//
	////CMD + ts + tok + ms + type + data
	//memcpy_s(writer,				sizeOfInt,	&command,		sizeOfInt);
	//memcpy_s(writer += sizeOfInt,	sizeOfInt,	&token_size,	sizeOfInt);
	//memcpy_s(writer += sizeOfInt,	token_size, token,			token_size);
	//memcpy_s(writer += token_size,	sizeOfInt,	&message_size,	sizeOfInt);
	//memcpy_s(writer += sizeOfInt,	sizeOfInt,	&type,			sizeOfInt);
	//memcpy_s(writer += sizeOfInt,	file_size,	data,			file_size);

	Header outHeader(command, header.token_size, header.token, message_size, data);
	send(clientSocket, outHeader.Serialize(), message_size, 0);
	
	const char* read_point = data;
	for (size_t bytesLeft = message_size; bytesLeft;)
	{
		const size_t packet_size = min(bytesLeft, PACKET_SIZE);
		

		send(clientSocket, message, (int)message_size, 0);
		bytesLeft -= 
	}

	//No clue why this delete is screwing things up
	//delete[] message;
	delete[] image_name;
}

unsigned int WindowsInterpreter::ReadByteHeader(const SOCKET& clientSocket)
{
	unsigned int byteHeader = 0;
	recv(clientSocket, (char*)&byteHeader, sizeOfInt, 0);
	return byteHeader;
}

bool WindowsInterpreter::VerifyUserAuth(const SOCKET& clientSocket, User& user)
{
	const unsigned int header = ReadByteHeader(clientSocket);
	bool success = false;

	if (header) {
		char* token = new char[header];

		recv(clientSocket, (char*)token, header, 0);
		return success = (FindUserByToken(token)) ? true : false;

		delete[] token;
	}
	return success;
}

bool WindowsInterpreter::EnsureSingleTokenInstance(const std::string& token)
{
	if (_clientPairs.find(token) == _clientPairs.end())
		return true;
	
	return false;
}

std::string WindowsInterpreter::CreateToken(User& user)
{
	std::string checkToken = user.Token();

	while (!EnsureSingleTokenInstance(checkToken)) {
		user.RemakeToken();
		checkToken = user.Token();

	}

	return checkToken;
}

void WindowsInterpreter::NewDiscussionPost(const SOCKET& clientSocket)
{
	static constexpr unsigned int command = (unsigned int)Command::NewDiscussionPost;

	// TODO: run more test to get the discussion posts sending out to other clients.
	User user;
	
	if (VerifyUserAuth(clientSocket, user)) {
		unsigned int byteHeader = ReadByteHeader(clientSocket);
		if (byteHeader > 0) {
			char* buffer = new char[byteHeader + 1];
			recv(clientSocket, buffer, byteHeader, 0);
			std::cout << buffer << '\n';

			DiscussionPost infoToSend = _commands.NewDiscussionPost(buffer, user, byteHeader);
			delete[] buffer;
			// distribution
			char messageToSend[1024];
			char* username = infoToSend.GetAuthor();
			const char* postDetails = infoToSend.GetPost();
			const size_t usernameSize = strlen(username) + 1;
			username[usernameSize - 1] = '\n';
			const size_t postSize = strlen(postDetails) + 1;
			const size_t packet = (unsigned int)(sizeOfInt + postSize + usernameSize + 1);
			const size_t length = usernameSize + postSize + sizeOfInt + sizeOfInt;

			const unsigned int detailHeader = (unsigned int)(usernameSize + postSize);
			//messageToSend = new char[packet];
			memcpy_s(messageToSend,									packet, &command,		sizeOfInt);
			memcpy_s(messageToSend + sizeOfInt,						packet, &detailHeader,	sizeOfInt);
			memcpy_s(messageToSend + sizeOfInt * 2,					packet, username,		usernameSize);
			memcpy_s(messageToSend + sizeOfInt * 2 + usernameSize,	packet, postDetails,	postSize);
			SendPostToClients(clientSocket, messageToSend, length);

			SendMessageToClient(clientSocket, true);
		}
	}
	else {
		SendMessageToClient(clientSocket, false);
	}
}

void WindowsInterpreter::SendPostToClients(const SOCKET& clientSocket, const char* buffer, const size_t sizeOfBuffer)
{
	// Does not entirely work
	char message[sizeOfInt] = { 0 };
	auto clientIter = _clientPairs.begin();
	
	for (; clientIter != _clientPairs.end(); clientIter++) {
		if (clientIter->second.clientSocket != clientSocket) {
			
			send(clientIter->second.clientSocket, buffer, (int)sizeOfBuffer, 0);
			recv(clientIter->second.clientSocket, message, sizeOfInt, 0);
			unsigned int readback = 0;
			memcpy_s(&readback, sizeOfInt, message, sizeOfInt);
			if (readback) {
				std::cout << "Success Readback\n";
			}
		}

	}

}

void WindowsInterpreter::ReceiveImage(const SOCKET& clientSocket)
{
	static constexpr const char file_ext[] = ".png";
	static constexpr unsigned int wait_period = 5; //seconds
	Header header(clientSocket, wait_period);
	
	const char* file_buffer = header.buffer;
	const unsigned int file_size = header.buffer_size;

	//Get file name
	EnvironmentFile* env = EnvironmentFile::Instance();
	const char* path = env->FetchEnvironmentVariable("file_save_path");

	if (!path) { std::cerr << "Unable to find the \"file_save_path\" variable in your .env file\n"; return; }

	const char* username = FindUserByToken(header.token)->Username();
	const size_t username_length = strlen(username);
	const size_t path_length = strlen(path);
	char* file_name = new char[path_length + username_length + sizeof(file_ext)];
	memcpy_s(file_name, path_length, path, path_length);
	memcpy_s(file_name + path_length, username_length, username, username_length);
	memcpy_s(file_name + path_length + username_length, sizeof(file_ext), file_ext, sizeof(file_ext));

	// Verify file
	if (!file_size) { std::cerr << "Did not receive a file.\n";  return;}
	std::ofstream file = std::ofstream(file_name, std::ios::binary);
	if (!file) { std::cerr << "Error opening file.\n"; return; }

	//Write to file
	file.write(file_buffer, file_size);
	file.close();
}

void WindowsInterpreter::LogOut(const SOCKET clientSocket)
{
	const unsigned int token_size = ReadByteHeader(clientSocket);
	char* token = new char[token_size + 1]; token[token_size] = '\0';
	recv(clientSocket, token, token_size, 0);
	const unsigned int message_size = ReadByteHeader(clientSocket);
	
	//Ignore whatever message might be associated here, totally unnecessary.
	if (message_size)
	{
		char* buffer = new char[message_size];
		recv(clientSocket, buffer, message_size, 0);
		delete[] buffer;
	}

	//Find and erase
	auto pair = _clientPairs.find(token);
	if (pair == _clientPairs.end())
	{
		std::cerr << "User token not found! Couldn't log out.\n";
		return;
	}
	std::cout << "User \"" << pair->second.user.Username() << "\" has signed out!\n";
	_clientPairs.erase(pair);
}

User* WindowsInterpreter::FindUserByToken(const char* token)
{
	const auto existingToken = _clientPairs.find(token);
	if (existingToken != _clientPairs.end())
		return &(existingToken->second.user);

	return nullptr;
}

User* WindowsInterpreter::FindUserByToken(const std::string& token)
{
	const auto existingToken = _clientPairs.find(token);
	if (existingToken != _clientPairs.end())
		return &(existingToken->second.user);

	return nullptr;
}


