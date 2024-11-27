#include "WindowsInterpreter.h"
#include "iostream"

#include <functional>
#include <thread>
#include <fstream>

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

//TODO: There is still a user with a gnarly username if this fails, we should make sure not to push bad clients to the user map.
//	interestingly, they aren't added to the DB, so that's good...
void WindowsInterpreter::HandleLoginUser(const SOCKET& clientSocket)
{

	unsigned int sizeOfHeader = ReadByteHeader(clientSocket);
	if (sizeOfHeader != 0) {
		char* buffer = new char[sizeOfHeader + 1];
		unsigned int bytesRead = recv(clientSocket, buffer, sizeOfHeader, 0);
		if (bytesRead != 0) {
			bool success = false;
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

		static constexpr unsigned int sizeOfToken = hashSize + 1;
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


	}
	else {
		static constexpr const char* failedAttempt = "Unauthorized username or password";
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
	if (success) {
		static constexpr const char* message = "Message Recieved";
		static constexpr unsigned int messageLength = sizeof(message);
		static constexpr unsigned int command = (unsigned int)MessageResult::Success;
		static constexpr unsigned int packetSize = messageLength + sizeOfInt * 2;
		
		char response[packetSize];
		memcpy_s(response, sizeOfInt, &command, sizeOfInt);
		memcpy_s(response + sizeOfInt, sizeOfInt, &messageLength, sizeOfInt);
		memcpy_s(response + sizeOfInt * 2, messageLength, message, messageLength);
		send(clientSocket, response, packetSize, 0);
	}
	else {
		static constexpr const char message[] = "Error reading message";
		static constexpr const unsigned int messageLength = sizeof(message);
		static constexpr const unsigned int command = (unsigned int)MessageResult::Failed;
		static constexpr const unsigned int packetSize = messageLength + sizeOfInt * 2;
		
		char response[packetSize];
		memcpy_s(response, sizeOfInt, &command, sizeOfInt);
		memcpy_s(response + sizeOfInt, sizeOfInt, &messageLength, sizeOfInt);
		memcpy_s(response + sizeOfInt * 2, messageLength, message, messageLength);
		send(clientSocket, response, packetSize, 0);
	}
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
		std::string tokenAsStr = token;
		auto existingToken = _clientPairs.find(tokenAsStr);
		if (existingToken != _clientPairs.end()) 
		{
			success = true;
			user = existingToken->second.user;
		}

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
			char* postDetails = infoToSend.GetPost();
			unsigned int usernameSize = strlen(username) + 1;
			username[usernameSize - 1] = '\n';
			unsigned int postSize = strlen(postDetails) + 1;
			unsigned int detailHeader = usernameSize + postSize;
			unsigned int command = (unsigned int) Command::NewDiscussionPost;
			unsigned int packet = sizeOfInt + postSize + usernameSize + 1;
			//messageToSend = new char[packet];
			memcpy_s(messageToSend, packet, &command, sizeOfInt);
			memcpy_s(messageToSend + sizeOfInt, packet, &detailHeader, sizeOfInt);
			memcpy_s(messageToSend + sizeOfInt * 2, packet, username, usernameSize);
			memcpy_s(messageToSend + sizeOfInt * 2 + usernameSize, packet, postDetails, postSize);
			int length = usernameSize + postSize + sizeOfInt + sizeOfInt;
			SendPostToClients(clientSocket, messageToSend, length);

			SendMessageToClient(clientSocket, true);
			//delete[] messageToSend;
		}
	}
	else {
		SendMessageToClient(clientSocket, false);
	}
}

void WindowsInterpreter::SendPostToClients(const SOCKET& clientSocket, const char* buffer, unsigned int sizeOfBuffer)
{
	// Does not entirely work
	char message[sizeOfInt] = { 0 };
	auto clientIter = _clientPairs.begin();
	
	for (; clientIter != _clientPairs.end(); clientIter++) {
		if (clientIter->second.clientSocket != clientSocket) {
			
			send(clientIter->second.clientSocket, buffer, sizeOfBuffer, 0);
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
	const unsigned int sizeOfHeader = ReadByteHeader(clientSocket);

	if (!sizeOfHeader)
		return;

	char* buffer = new char[sizeOfHeader];
	recv(clientSocket, buffer, sizeOfHeader, 0);

	//TODO: This is the result of a systemic problem, I'll have to find an efficient way to get the User so we can use the username as the filename
	static constexpr const char TMPFILENAME[] = "temporary_file_name.png";

	std::ofstream file = std::ofstream(TMPFILENAME, std::ios::binary);
	if (!file)
	{
		std::cerr << "Error opening file.\n";
		return;
	}

	file.write(buffer, sizeOfHeader);
	file.close();
	delete[] buffer;
}

