#pragma once
#include "WindowsUserPair.h"
#include "Constants.h"
#include "CommandSet.h"
#include "DiscussionPost.h"
#include "Header.h"

#include <WinSock2.h>
#include <unordered_map>
#include <string>
// This class will handle communication and distribution of information
// to clients. The pair vector will be here so if we need to contact a client
// from another client we can loop through and find the information.
// This prevents the clutter on the server which already has a lot in its cpp file
class WindowsInterpreter
{
public:
	//This will take the client socket and begin interpretations.
	//From here the appropriate functions will be called and data handled outside of the switch statement.
	void InterpretMessage(const SOCKET &clientSocket, Command command);
	void DisconnectClient(const SOCKET& clientSocket);
private:

	void HandleLoginUser(const SOCKET& clientSocket);
	void HandleNewUser(const SOCKET& clientSocket);
	void LoginResponseToUser(const SOCKET& clientSocket, User& user, const bool success);
	// This may get reworked to handle direct messaged or deleted in general.
	// Another Method needs to be responsible for handling discussion posts
	// discussion posts have more nuances to handle like is it a comment or main post.
	void MessageToServer(const SOCKET& clientSocket);
	void SendMessageToClient(const SOCKET& clientSocket, bool success);
	unsigned int ReadByteHeader(const SOCKET& clientSocket);
	bool VerifyUserAuth(const SOCKET& clientSocket, User& user);
	bool EnsureSingleTokenInstance(const std::string& token);
	std::string CreateToken(User& user);
	void NewDiscussionPost(const SOCKET& clientSocket);
	void SendPostToClients(const SOCKET&clientSocket, const char* buffer, const size_t sizeOfBuffer);
	void CreateMessagePacket(DiscussionPost& post , const size_t packetSize);
	void ReceiveImage(const SOCKET& clientSocket);
	void SendData(const SOCKET clientSocket);
	void SendImage(const SOCKET clientSocket, const Header& header);
	void LogOut(const SOCKET clientSocket);
	User* FindUserByToken(const char* clientSocket);
	User* FindUserByToken (const std::string& clientSocket);

	//					Token		User+Socket+*token
	std::unordered_map<std::string, WindowsUserPair> _clientPairs;
	CommandSet _commands;
};

