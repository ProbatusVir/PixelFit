#pragma once
#include <unordered_map>
#include <string>
#include "WindowsUserPair.h"

//This *coudl* use abstraction, but making a linux server is beyond our scope
class ActiveUsers
{
public:
	std::unordered_map<std::string, WindowsUserPair>& data();
	bool VerifyUserAuth(const char* token) const;
	bool EnsureSingleTokenInstance(const std::string& token) const;

	const User* FindUserByToken(const char* clientSocket) const;
	User* FindUserByToken(const char* clientSocket);
	const User* FindUserByToken(const std::string& clientSocket) const;
	User* FindUserByToken(const std::string& clientSocket);
private:
	//					Token		User+Socket+*token
	std::unordered_map<std::string, WindowsUserPair> _clientPairs;
	 ActiveUsers() = default;
	~ActiveUsers() = default;

public:
	static ActiveUsers* Instance() { if (!instance) return new ActiveUsers(); else return instance; }
	static void DestroyInstance() {delete instance;}

private:
	static ActiveUsers* instance;

};

