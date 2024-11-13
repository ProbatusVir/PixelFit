#pragma once
#include <openssl/evp.h>
#include <openssl/sha.h>
#include "Constants.h"

class User
{
public:
	User(const char name[nameSize], const char userName[usernameSize], const char password[passwordSize]);
	User();
	~User();

private:

	unsigned char* HashPassword(const char password[passwordSize]);
	char _name[nameSize];
	char _userName[usernameSize];
	unsigned char* _password;
	bool _errorOnCreation;
	unsigned char _token[hashSize + 1];


};

