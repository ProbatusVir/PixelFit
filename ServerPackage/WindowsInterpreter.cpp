#include "WindowsInterpreter.h"
#include "FileOps.h"

#include <iostream>
#include <functional>
#include <thread>
#include <fstream>
#include <chrono>
#include <filesystem>
#include "SQLInterface.h"

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

	case Command::SendPfpToServer:
		ReceivePfp(clientSocket);
		break;
	case Command::RequestData: 
		SendData(clientSocket);
		break;
	case Command::LogOut:
		LogOut(clientSocket);
		break;
	case Command::GetAllUsers :
		GetAllUsers(clientSocket);
		break;
	case Command::GetUsersContaining :
		GetUsersContaining(clientSocket);
		break;
	case Command::GetActiveUsers :
		GetActiveUsers(clientSocket);
		break;
	}
}

void SendPiecewise(const SOCKET clientSocket, const char* data, const size_t message_size) {
	for (size_t bytes_left = message_size; bytes_left;)
	{
		const size_t packet_size = min(bytes_left, PACKET_SIZE);

		send(clientSocket, data, (int)packet_size, 0);
		bytes_left -= packet_size;
		data += packet_size;
	}

}

void SimpleFileSend(const SOCKET clientSocket, const InboundPacket& header, const ResourceType type, const char* suffix) {
	constexpr static const Command command = Command::RequestData;
	const char* reader = header.buffer + sizeof(int);

	FileOps fop = FileOps();

	const size_t file_stem_length = strlen(reader);
	char* file_stem = new char[file_stem_length + 2]; file_stem[file_stem_length] = '\0'; file_stem[file_stem_length + 1] = '\0';
	memcpy(file_stem, reader, file_stem_length);

	const char* file_path = EnvironmentFile::Instance()->FetchEnvironmentVariable("file_save_path");

	const char* components[] = { file_path, file_stem, suffix };
	const char* file_name = CONCATENATE(components);
	file_stem[file_stem_length] = '\n';

	const char* data = fop.ReadFullFile(file_name, false);
	const size_t file_size = fop.FileSize();

	//Create message

	const unsigned int message_size = (unsigned int)(sizeof(type) + (file_stem_length + 1) + file_size ); //no null


	OutboundHeader outHeader(command, header.token_size, header.token, message_size);

	const char* out = outHeader.Serialize();

	send(clientSocket, out, (int)outHeader.serialized_size, 0);
	send(clientSocket, (char*)&type, sizeof(type), 0);
	send(clientSocket, file_stem, (int)file_stem_length + 1, 0);
	SendPiecewise(clientSocket, data, file_size);

	delete[] file_stem;
	delete[] file_name;
}

void WindowsInterpreter::DisconnectClient(const SOCKET& clientSocket)
{
	if (_clientPairs.size()) {
		for (auto client = _clientPairs.begin(); client != _clientPairs.end();) {
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
	const unsigned int tokenSize = ReadByteHeader(clientSocket);
	const unsigned int sizeOfHeader = ReadByteHeader(clientSocket);
	if (sizeOfHeader != 0) {
		char* buffer = new char[sizeOfHeader + 1];
		const unsigned int bytesRead = recv(clientSocket, buffer, sizeOfHeader, 0);
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

		constexpr unsigned int sizeOfToken = HASH_SIZE + 1;
		constexpr unsigned int sizeOfResponse = sizeOfInt * 2 + sizeOfToken;
		constexpr unsigned int msgSuccess = (int)MessageResult::LoginSuccess;
		char response[sizeOfResponse];

		const std::string tokenAsStr = CreateToken(user);
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

		const std::pair<std::string, WindowsUserPair> newPair(tokenAsStr, clientPair);
		_clientPairs.insert(newPair);

		std::cout << "Successfully logged in user \"" << user.Username() << "\"!\n";

	}
	else {
		constexpr const char failedAttempt[] = "Unauthorized username or password";
		constexpr const unsigned int sizeOfResponse = sizeOfInt * 2 + sizeof(failedAttempt);
		constexpr unsigned int failed = (unsigned int)MessageResult::Failed;
		constexpr unsigned int lengthOfFailedAttempt = sizeof(failedAttempt) - 1;

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
		const unsigned int sizeOfHeader = ReadByteHeader(clientSocket);
		bool success = false;
		char* buffer = new char[sizeOfHeader + 1];

		const int bytesRead = recv(clientSocket, buffer, sizeOfHeader, 0);
		if (bytesRead) {
			success = true;
			std::cout << buffer << '\n';
			SendMessageToClient(clientSocket, success);
		}
		else SendMessageToClient(clientSocket, success);

	}
	else SendMessageToClient(clientSocket, false);
}

void WindowsInterpreter::SendMessageToClient(const SOCKET& clientSocket, bool success)
{
	constexpr const char success_message[] = "Message Received";
	constexpr const char fail_message[] = "Error reading message";
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
	InboundPacket header(clientSocket);

	const char* reader = header.buffer;
	int resource_type = 0;
	memcpy_s(&resource_type, sizeOfInt, reader, sizeOfInt);
	reader += sizeOfInt;

	switch (resource_type)
	{
	case (int)ResourceType::PNG:
		SendImage(clientSocket, header);
		break;
	case (int)ResourceType::DIR:
		SendDirectory(clientSocket, header);
		break;
	case (int)ResourceType::WORK:
		SendWork(clientSocket, header);
		break;
	}
}

//Prolly gonna send the user's token back to them at some point, just to verify this message, but for now... meh
//FIXME: make me actually work, there's some funnky stuff going on with getting the image name, and that's the only problem...
void WindowsInterpreter::SendImage(const SOCKET clientSocket, const InboundPacket& header)
{
	SimpleFileSend(clientSocket, header, ResourceType::PNG, ".png");
}

void WindowsInterpreter::SendDirectory(const SOCKET clientSocket, const InboundPacket& header)
{
	constexpr static const Command command = Command::RequestData;
	constexpr static const ResourceType type = ResourceType::DIR;
	const char* reader = header.buffer + sizeof(int);

	FileOps fop = FileOps();

	const char* file_path = EnvironmentFile::Instance()->FetchEnvironmentVariable("file_save_path");

	const char* components[] = { file_path, reader };
	const char* full_directory = CONCATENATE(components);

	std::string data = std::string(reader) + '\n';

	if (!std::filesystem::is_directory(full_directory))
	{
		std::cerr << "Directory \"" << full_directory << "\" not found!\n";
		return;
	}

	for (const auto& entry : std::filesystem::directory_iterator(full_directory))
		data += entry.path().stem().generic_string() + '\n';

	const unsigned int message_size = (unsigned int)(sizeof(type) + data.length());

	OutboundHeader outHeader (command, header.token_size, header.token, message_size);

	const char* out = outHeader.Serialize();

	send(clientSocket, out, (int)outHeader.serialized_size, 0);
	send(clientSocket, (char*)&type, sizeof(type), 0);

	const char* read_point = data.data();
	for (size_t bytes_left = message_size; bytes_left;)
	{
		const size_t packet_size = min(bytes_left, PACKET_SIZE);

		send(clientSocket, read_point, (int)packet_size, 0);
		bytes_left -= packet_size;
		read_point += packet_size;
	}
}

void WindowsInterpreter::SendWork(const SOCKET clientSocket, const InboundPacket& header)
{
	SimpleFileSend(clientSocket, header, ResourceType::WORK, ".work");
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
		const unsigned int byteHeader = ReadByteHeader(clientSocket);
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

void WindowsInterpreter::ReceivePfp(const SOCKET& clientSocket)
{
	InboundPacket header(clientSocket, 5);
	
	const char* file_buffer = header.buffer;
	const unsigned int file_size = header.buffer_size;
	
	//Get file name
	EnvironmentFile* env = EnvironmentFile::Instance();
	const char* path = env->FetchEnvironmentVariable("file_save_path");

	if (!path) { std::cerr << "Unable to find the \"file_save_path\" variable in your .env file\n"; return; }

	const char* username = FindUserByToken(header.token)->Username();

	const char* components[] = { path, username, ".png" };
	const char* file_name = CONCATENATE(components);

	// Verify file
	if (!file_size) { std::cerr << "Did not receive a file.\n";  return;}
	std::ofstream file = std::ofstream(file_name, std::ios::binary);
	if (!file) { std::cerr << "Error opening file.\n"; return; }

	//Write to file
	file.write(file_buffer, file_size);
	file.close();
}

void WindowsInterpreter::GetAllUsers(const SOCKET clientSocket)
{
	InboundPacket header(clientSocket);
	std::vector<std::string> usernames = SQLInterface::Instance()->GetEveryExistingUsername();
	std::string message;

	for (std::string& username : usernames)
	{
		const size_t whitespace = username.find(' ');
		if (whitespace != std::string::npos)
			username.erase(whitespace);

		message += username + '\n';
	}
	message.pop_back();
	
	OutboundHeader outHeader(Command::GetAllUsers, header.token_size, header.token, message.size());
	outHeader.Serialize();
	send(clientSocket, outHeader.serialized, outHeader.serialized_size, 0);
	SendPiecewise(clientSocket, message.data(), message.size());

}

void WindowsInterpreter::GetActiveUsers(const SOCKET clientSocket) {}

void WindowsInterpreter::GetUsersContaining(const SOCKET clientSocket) {}

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


