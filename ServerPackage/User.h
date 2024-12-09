#pragma once
#include <openssl/evp.h>
#include <openssl/sha.h>
#include "Constants.h"

class User
{
public:
	User(const char userName[USERNAME_SIZE]);
	User();
	User(const User& user);
	~User();
	const char* Username() const { return _userName; } ;
	char* Token() { return _token; };
	void RemakeToken();

	static char* HashPassword(const char password[PASSWORD_SIZE]);


private:
	void CreateToken();
	char _userName[USERNAME_SIZE + 1];
	bool _errorOnCreation = false;
	char _token[HASH_SIZE + 1] = { 0 };

};

