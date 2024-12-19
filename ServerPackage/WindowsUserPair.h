#pragma once
#include "User.h"
#include <WinSock2.h>

struct WindowsUserPair {
	User user;
	SOCKET clientSocket;
	char* token = nullptr;

	WindowsUserPair(User _user, SOCKET _clientSocket, char* _token = nullptr) : user(_user), clientSocket(_clientSocket), token(_token) {};
};