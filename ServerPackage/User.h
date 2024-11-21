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
	char* Username() { return _userName; };
	char* Token() { return _token; }
	void RemakeToken();

	uint64_t Id() { return _id; }
	static char* HashPassword(const char password[passwordSize]);


private:
	void CreateToken();
	char _name[nameSize];
	char _userName[usernameSize];
	// This should be hash size as we want to save the hash, not the actual password
	char _password[hashSize] = { 0 };
	bool _errorOnCreation = false;
	char _token[hashSize + 1] = { 0 };
	uint64_t _id;

};

