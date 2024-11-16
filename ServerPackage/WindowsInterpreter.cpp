#include "WindowsInterpreter.h"
#include "iostream"

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
			if (client->clientSocket == clientSocket) {
				client = _clientPairs.erase(client);
			}
			else client++;
		}

	}

	closesocket(clientSocket);
}

void WindowsInterpreter::HandleLoginUser(const SOCKET& clientSocket)
{

	// This may break, it would be nice if it worked.

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
		const unsigned char* token = user.Token();
		unsigned int sizeOfToken = strlen((char*)token);
		// This is necessary for our response array to be sized with null terminator
		sizeOfToken++;
		unsigned int sizeOfResponse = sizeOfInt * 2 + sizeOfToken;
		response = new char[sizeOfResponse];
		unsigned int msgSuccess = (int)MessageResult::Success;
		memcpy_s(response, sizeOfInt, &msgSuccess, sizeOfInt);
		memcpy_s(response + sizeOfInt, sizeOfInt, &sizeOfToken, sizeOfInt);
		memcpy_s(response + sizeOfInt * 2, sizeOfToken, token, sizeOfToken);
		send(clientSocket, response, sizeOfResponse, 0);
		WindowsUserPair clientPair;
		clientPair.user = user;
		clientPair.clientSocket = clientSocket;
		_clientPairs.push_back(clientPair);

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

void WindowsInterpreter::SendMessageToClient(const SOCKET& clientSocket, bool success)
{
	if (success) {
		char message[] = "Message Recieved\0";
		unsigned int messageLength = strlen((char*)message);
		// idk why this works, but it works
		messageLength += 4;
		unsigned int responseCode = (unsigned int)MessageResult::Success;

		char* response = nullptr;
		unsigned int packetSize = messageLength + sizeOfInt + 1;
		response = new char[packetSize];
		memcpy_s(response, sizeOfInt, &responseCode, sizeOfInt);
		memcpy_s(response + sizeOfInt, sizeOfInt, &messageLength, sizeOfInt);
		memcpy_s(response + sizeOfInt * 2, messageLength, message, messageLength);
		send(clientSocket, response, packetSize, 0);

	}
	else {
		char message[] = "Error reading message\0";
		unsigned int messageLength = strlen(message);
		// Copies from the if statement, so again idk why its off by 4 but it is
		messageLength += 4;
		unsigned int responseCode = (unsigned int)MessageResult::Failed;

		char* response = nullptr;
		unsigned int packetSize = messageLength + responseCode + 1;
		response = new char[packetSize];
		memcpy_s(response, sizeOfInt, &responseCode, sizeOfInt);
		memcpy_s(response + sizeOfInt, sizeOfInt, &messageLength, sizeOfInt);
		memcpy_s(response + sizeOfInt * 2, messageLength, message, messageLength);
		send(clientSocket, response, packetSize, 0);
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


