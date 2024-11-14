#pragma once
#include "User.h"
#include <WinSock2.h>

struct WindowsUserPair {
	

	User user;
	SOCKET clientSocket;
};