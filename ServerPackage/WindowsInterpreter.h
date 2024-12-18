#pragma once
#include "WindowsUserPair.h"
#include "Constants.h"
#include "CommandSet.h"
#include "DiscussionPost.h"
#include "Header.h"
#include "ActiveUsers.h"

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
	void InterpretMessage(const SOCKET clientSocket, const Command command);
	void DisconnectClient(const SOCKET clientSocket);
	static unsigned int ReadByteHeader(const SOCKET clientSocket);
private:

	void HandleLoginUser(const SOCKET clientSocket);
	void HandleNewUser(const SOCKET clientSocket);
	void LoginResponseToUser(const SOCKET clientSocket, User& user, const bool success);
	// This may get reworked to handle direct messaged or deleted in general.
	// Another Method needs to be responsible for handling discussion posts
	// discussion posts have more nuances to handle like is it a comment or main post.
	void MessageToServer(const SOCKET clientSocket) const;
	void SendMessageToClient(const SOCKET clientSocket, bool success) const;
	std::string CreateToken(User& user) const;
	void NewDiscussionPost(const SOCKET clientSocket);
	void SendPostToClients(const SOCKET clientSocket, const char* buffer, const size_t sizeOfBuffer) const;
	void CreateMessagePacket(DiscussionPost& post , const size_t packetSize) const;
	void ReceivePfp(const SOCKET clientSocket) const;
	void SendData(const SOCKET clientSocket) const;
	void SendImage(const SOCKET clientSocket, const InboundPacket& header) const;
	void SendDirectory(const SOCKET clientSocket, const InboundPacket& header) const;
	void SendWork(const SOCKET clientSocket, const InboundPacket& header) const;
	void GetActiveUsers(const SOCKET clientSocket) const;
	void GetUsersContaining(const SOCKET clientSocket) const;
	void GetAllUsers(const SOCKET clientSocket) const;
	void LogOut(const SOCKET clientSocket);

	WindowsInterpreter() { _active_users = ActiveUsers::Instance(); }

	ActiveUsers* _active_users;
	CommandSet _commands;

public:
	static WindowsInterpreter* Instance() { if (instance == nullptr) return new WindowsInterpreter(); else return instance; };
	static void DestroyInstance() {	delete instance; }
private:
	static WindowsInterpreter* instance;
};

