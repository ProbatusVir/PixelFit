#pragma once
#include <openssl/evp.h>
#include <openssl/sha.h>
#include "Constants.h"

class User
{
public:
	User(const char userName[usernameSize]);
	User();
	User(const User& user);
	~User();
	const char* Username() const { return _userName; } ;
	char* Token() { return _token; };
	void RemakeToken();

	static char* HashPassword(const char password[passwordSize]);


private:
	void CreateToken();
	char _userName[usernameSize + 1];
	bool _errorOnCreation = false;
	char _token[hashSize + 1] = { 0 };

};

