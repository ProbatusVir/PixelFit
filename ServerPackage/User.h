#pragma once
#include <openssl/evp.h>
#include <openssl/sha.h>
#include "Constants.h"

class User
{
public:
	User(const char name[nameSize], const char userName[usernameSize], const char password[passwordSize], uint64_t id);
	User();
	User(const User& user);
	~User();
	uint64_t Id() { return _id; }
	unsigned char* HashPassword(const char password[passwordSize]);

	
private:

	char _name[nameSize];
	char _userName[usernameSize];
	unsigned char* _password;
	bool _errorOnCreation = false;
	unsigned char _token[hashSize + 1];
	uint64_t _id;

};

