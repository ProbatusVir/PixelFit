#pragma once
#include <openssl/evp.h>
#include <openssl/sha.h>

class User
{
public:
	User(char name[50], char userName[20], char password[60]);
	User();
	~User();

private:

	unsigned char* HashPassword(char password[60]);
	char _name[50];
	char _userName[20];
	char _password[SHA256_DIGEST_LENGTH + 1];
	bool _errorOnCreation = false;
	char _token[SHA256_DIGEST_LENGTH + 1];


};

