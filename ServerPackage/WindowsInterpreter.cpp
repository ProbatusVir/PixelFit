#include "WindowsInterpreter.h"
#include "iostream"

#include <functional>
#include <thread>

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
			bool success = false;
			User user = _commands.LoginUser(buffer, success);
			LoginResponseToUser(clientSocket, user, success);
		}

		if (buffer != nullptr) delete[] buffer;
	}
}

void WindowsInterpreter::HandleNewUser(const SOCKET& clientSocket)
{
	unsigned int sizeOfHeader = ReadByteHeader(clientSocket);

	if (sizeOfHeader != 0) {
		char* buffer = nullptr;
		buffer = new char[sizeOfHeader + 1];
		unsigned int bytesRead = recv(clientSocket, buffer, sizeOfHeader, 0);
		if (bytesRead != 0) {
			bool success = false;
			User newUser = _commands.NewUser(buffer, success);
			LoginResponseToUser(clientSocket, newUser, success);


		}
		if (buffer != nullptr) delete[] buffer;
	}

}

void WindowsInterpreter::LoginResponseToUser(const SOCKET& clientSocket, User& user, const bool success)
{
	char* response = nullptr;
	if (success) {

		std::string tokenAsStr = CreateToken(user);

		char* token = user.Token();
		unsigned int sizeOfToken = strlen(token) + 1;
		// This is necessary for our response array to be sized with null terminator
		unsigned int sizeOfResponse = sizeOfInt * 2 + sizeOfToken;
		response = new char[sizeOfResponse];
		unsigned int msgSuccess = (int)MessageResult::LoginSuccess;
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
		char failedAttempt[] = "Unauthorized username or password \0";
		unsigned int sizeOfResponse = sizeOfInt * 2 + strlen(failedAttempt) + 1;
		response = new char[sizeOfResponse];
		unsigned int failed = (unsigned int)MessageResult::Failed;
		unsigned int lengthOfFailedAttempt = strlen(failedAttempt);
		memcpy_s(response, sizeOfInt, &failed, sizeOfInt);
		memcpy_s(response + sizeOfInt, sizeOfInt, &lengthOfFailedAttempt, sizeOfInt);
		memcpy_s(response + sizeOfInt * 2, lengthOfFailedAttempt, failedAttempt, lengthOfFailedAttempt);
		send(clientSocket, response, sizeOfResponse, 0);
	}
	if (response != nullptr) delete[] response;
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

void WindowsInterpreter::SendMessageToClient(const SOCKET& clientSocket, bool success)
{
	if (success) {
		char message[] = "Message Recieved\0";
		unsigned int messageLength = strlen((char*)message);

		messageLength++;

		unsigned int command = (unsigned int)MessageResult::Success;

		char* response = nullptr;
		unsigned int packetSize = messageLength + sizeOfInt * 2;
		response = new char[packetSize];
		memcpy_s(response, sizeOfInt, &command, sizeOfInt);
		memcpy_s(response + sizeOfInt, sizeOfInt, &messageLength, sizeOfInt);
		memcpy_s(response + sizeOfInt * 2, messageLength, message, messageLength);
		send(clientSocket, response, packetSize, 0);
		delete[] response;

	}
	else {
		char message[] = "Error reading message\0";
		unsigned int messageLength = strlen((char*)message);

		messageLength++;

		unsigned int command = (unsigned int)MessageResult::Failed;

		char* response = nullptr;
		unsigned int packetSize = messageLength + sizeOfInt * 2;
		response = new char[packetSize];
		memcpy_s(response, sizeOfInt, &command, sizeOfInt);
		memcpy_s(response + sizeOfInt, sizeOfInt, &messageLength, sizeOfInt);
		memcpy_s(response + sizeOfInt * 2, messageLength, message, messageLength);
		send(clientSocket, response, packetSize, 0);
		delete[] response;
	}
}

unsigned int WindowsInterpreter::ReadByteHeader(const SOCKET& clientSocket)
{
	unsigned int byteHeader = 0;
	char response[4] = { 0 };
	// This may break, it would be nice if it worked.

	recv(clientSocket, response, sizeOfInt, 0);
	byteHeader = (unsigned int)response[0];

	return byteHeader;
}

bool WindowsInterpreter::VerifyUserAuth(const SOCKET& clientSocket, User& user)
{
	unsigned int header = ReadByteHeader(clientSocket);
	bool success = false;
	if (header) {
		char* token = nullptr;
		token = new char[header];

		recv(clientSocket, (char*)token, header, 0);
		std::string tokenAsStr = token;
		auto existingToken = _clientPairs.find(tokenAsStr);
		if (existingToken == _clientPairs.end()) {

		}
		else {
			success = true;
			user = existingToken->second.user;
		}

		if (token != nullptr) delete[] token;
	}
	return success;
}

bool WindowsInterpreter::EnsureSingleTokenInstance(std::string token)
{
	auto isValid = _clientPairs.find(token);
	if (isValid == _clientPairs.end()) {
		return true;
	}
	else return false;
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






