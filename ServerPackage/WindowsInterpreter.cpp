#include "WindowsInterpreter.h"
#include "FileOps.h"

#include <iostream>
#include <functional>
#include <thread>
#include <fstream>
#include <chrono>
#include <filesystem>
#include "SQLInterface.h"
#include "TokenHelper.h"
#include "Challenges.h"

WindowsInterpreter* WindowsInterpreter::instance = nullptr;


void WindowsInterpreter::InterpretMessage(const SOCKET clientSocket, const Command command)
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
	default : 
		std::cerr << "Invalid command\n";
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

void SimpleFileSend(const SOCKET clientSocket, const InboundResourcePacket& header, const ResourceType type, const char* suffix) {
	constexpr static const Command command = Command::RequestData;

	FileOps fop = FileOps();

	const size_t file_stem_length = strlen(header.buffer);
	char* file_stem = new char[file_stem_length + 2]; file_stem[file_stem_length] = '\0'; file_stem[file_stem_length + 1] = '\0';
	memcpy(file_stem, header.buffer, file_stem_length);

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

bool VerifyUserAuth(const SOCKET clientSocket, User& user)
{
	const unsigned int header = WindowsInterpreter::Instance()->ReadByteHeader(clientSocket);
	bool success = false;

	if (header) {
		char* token = new char[header];

		recv(clientSocket, (char*)token, header, 0);
		return success = (ActiveUsers::Instance()->FindUserByToken(token)) ? true : false;

		delete[] token;
	}
	return success;
}

void WindowsInterpreter::DisconnectClient(const SOCKET clientSocket)
{
	std::unordered_map<std::string, WindowsUserPair>& _clientPairs = _active_users->data();
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

void WindowsInterpreter::HandleLoginUser(const SOCKET clientSocket)
{
	const InboundPacket in(clientSocket);
	
	if (in.buffer_size == 0)
	{
		SendMessageToClient(clientSocket, false);
		return;
	}

	bool success = true;
	User user = _commands.LoginUser(in.buffer, success);
	LoginResponseToUser(clientSocket, user, success);

}

void WindowsInterpreter::HandleNewUser(const SOCKET clientSocket)
{
	InboundPacket in(clientSocket);
	if (in.buffer_size == 0)
	{
		SendMessageToClient(clientSocket, false);
		return;
	}

	bool success = true;
	User newUser = _commands.NewUser(in.buffer, success);
	LoginResponseToUser(clientSocket, newUser, success);
}

void WindowsInterpreter::LoginResponseToUser(const SOCKET clientSocket, User& user, const bool success)
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
		WindowsUserPair clientPair(user, clientSocket, token);

		const std::pair<std::string, WindowsUserPair> newPair(tokenAsStr, clientPair);
		_active_users->data().insert(newPair);

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

void WindowsInterpreter::MessageToServer(const SOCKET clientSocket) const
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

void WindowsInterpreter::SendMessageToClient(const SOCKET clientSocket, bool success) const
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

void WindowsInterpreter::SendData(const SOCKET clientSocket) const
{
	const InboundResourcePacket header(clientSocket);

	const ResourceType type = header.type;
	switch (header.type)
	{
	case ResourceType::PNG:
		SendImage(clientSocket, header);
		break;
	case ResourceType::DIR:
		SendDirectory(clientSocket, header);
		break;
	case ResourceType::WORK:
		SendWork(clientSocket, header);
		break;
	default:
		std::cerr << "Invalid resource type.\n";
	}
}

void WindowsInterpreter::SendImage(const SOCKET clientSocket, const InboundResourcePacket& header) const
{
	SimpleFileSend(clientSocket, header, header.type, ".png");
}

void WindowsInterpreter::SendDirectory(const SOCKET clientSocket, const InboundResourcePacket& header) const
{
	constexpr static const Command command = Command::RequestData;

	FileOps fop = FileOps();

	const char* file_path = EnvironmentFile::Instance()->FetchEnvironmentVariable("file_save_path");

	const char* components[] = { file_path, header.buffer };
	const char* full_directory = CONCATENATE(components);

	std::string data = std::string(header.buffer) + '\n';

	if (!std::filesystem::is_directory(full_directory))
	{
		std::cerr << "Directory \"" << full_directory << "\" not found!\n";
		return;
	}

	for (const auto& entry : std::filesystem::directory_iterator(full_directory))
		data += entry.path().stem().generic_string() + '\n';

	const unsigned int message_size = (unsigned int)(sizeof(header.type) + data.length());

	OutboundHeader outHeader (command, header.token_size, header.token, message_size);

	const char* out = outHeader.Serialize();

	send(clientSocket, out, (int)outHeader.serialized_size, 0);
	send(clientSocket, (char*)&header.type, sizeof(header.type), 0);

	const char* read_point = data.data();
	for (size_t bytes_left = message_size; bytes_left;)
	{
		const size_t packet_size = min(bytes_left, PACKET_SIZE);

		send(clientSocket, read_point, (int)packet_size, 0);
		bytes_left -= packet_size;
		read_point += packet_size;
	}
}

void WindowsInterpreter::SendWork(const SOCKET clientSocket, const InboundResourcePacket& header) const
{
	SimpleFileSend(clientSocket, header, header.type, ".work");
}

unsigned int WindowsInterpreter::ReadByteHeader(const SOCKET clientSocket)
{
	unsigned int byteHeader = 0;
	recv(clientSocket, (char*)&byteHeader, sizeOfInt, 0);
	return byteHeader;
}

std::string WindowsInterpreter::CreateToken(User& user) const
{
	std::string checkToken = user.Token();

	while (!_active_users->EnsureSingleTokenInstance(checkToken)) {
		user.RemakeToken();
		checkToken = user.Token();

	}

	return checkToken;
}

void WindowsInterpreter::NewDiscussionPost(const SOCKET clientSocket)
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

void WindowsInterpreter::SendPostToClients(const SOCKET clientSocket, const char* buffer, const size_t sizeOfBuffer) const
{
	// Does not entirely work
	std::unordered_map<std::string, WindowsUserPair>& _clientPairs = _active_users->data();
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

void WindowsInterpreter::ReceivePfp(const SOCKET clientSocket) const
{
	const InboundPacket header(clientSocket, 5);
	
	if (!header.token) return;
	
	const char* file_buffer = header.buffer;
	const unsigned int file_size = header.buffer_size;
	
	//Get file name
	EnvironmentFile* env = EnvironmentFile::Instance();
	const char* path = env->FetchEnvironmentVariable("file_save_path");

	if (!path) { std::cerr << "Unable to find the \"file_save_path\" variable in your .env file\n"; return; }

	const char* username = _active_users->FindUserByToken(header.token)->Username();

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

void WindowsInterpreter::ReceiveChallenges(const SOCKET clientSocket) const
{
	InboundResourcePacket in(clientSocket);
	const char* reader;
	//Challenges::challenges.push_back(new Challenge(in.buffer, in.buffer_size, reader));
}

void WindowsInterpreter::GetAllUsers(const SOCKET clientSocket) const
{
	const InboundPacket header(clientSocket);
	std::vector<std::string> usernames = SQLInterface::Instance()->GetEveryExistingUsername();
	std::string message;

	for (std::string& username : usernames)
	{
		trimstr(message);

		message += username + '\n';
	}
	if (!message.empty())
		message.pop_back();
	
	OutboundHeader outHeader(Command::GetAllUsers, (int)header.token_size, header.token, (int)message.size());
	outHeader.Serialize();
	send(clientSocket, outHeader.serialized, (int)outHeader.serialized_size, 0);
	SendPiecewise(clientSocket, message.data(), message.size());

}

void WindowsInterpreter::GetActiveUsers(const SOCKET clientSocket) const {
	//TOKEN, user+socket+*token
	const InboundPacket header(clientSocket);
	std::string message;


	for (const std::pair<std::string, WindowsUserPair>& client : _active_users->data())
	{
		const WindowsUserPair& current = client.second;
		const char* username = current.user.Username();

		if (current.clientSocket != clientSocket)
			message += std::string(username) + '\n';
	}
	if (!message.empty())
		message.pop_back();

	OutboundHeader outHeader(Command::GetActiveUsers, header.token_size, header.token, (int)message.size());
	outHeader.Serialize();
	send(clientSocket, outHeader.serialized, (int)outHeader.serialized_size, 0);
	SendPiecewise(clientSocket, message.data(), message.size());
}

void WindowsInterpreter::GetUsersContaining(const SOCKET clientSocket) const {
	const InboundPacket in(clientSocket);
	std::vector<std::string> users = SQLInterface::Instance()->GetEveryUserContaining(in.buffer);
	std::string message;
	//const User initiater = FindUserByToken(in.token);
	for (std::string& user : users)
		message += trimstr(user) + '\n';

	if (!message.empty())
		message.pop_back();

	OutboundHeader out(Command::GetUsersContaining, in.token_size, in.token, (int)message.size());
	out.Serialize();

	send(clientSocket, out.serialized, (int)out.serialized_size, 0);
	SendPiecewise(clientSocket, message.data(), out.buffer_size);

}

void WindowsInterpreter::LogOut(const SOCKET clientSocket)
{
	InboundPacket in(clientSocket);

	//Find and erase
	std::unordered_map<std::string, WindowsUserPair>& _clientPairs = _active_users->data();

	auto pair = _clientPairs.find(in.token);
	if (pair == _clientPairs.end())
	{
		std::cerr << "User token not found! Couldn't log out.\n";
		return;
	}
	std::cout << "User \"" << pair->second.user.Username() << "\" has signed out!\n";
	_clientPairs.erase(pair);
}

