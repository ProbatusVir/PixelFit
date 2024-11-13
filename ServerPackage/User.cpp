#include "User.h"
#include <string>




User::User(const char name[nameSize], const char userName[usernameSize], const char password[passwordSize])
{
	strcpy_s(_name, name);
	strcpy_s(_userName, userName);
	unsigned char* hashed = HashPassword(password);

	if (hashed == nullptr) _errorOnCreation = true;
	else {
		_password = hashed;
	}


}

User::User()
{
}

User::~User()
{
	delete[] _password;
}

unsigned char* User::HashPassword(const char password[passwordSize])
{
	unsigned char* hash = new unsigned char[hashSize + 1]{ 0 };
	EVP_MD_CTX* context = EVP_MD_CTX_new();
	const EVP_MD* sha256 = EVP_sha256();
	unsigned int lengthOfHash = 0;
	if (context == nullptr) {
		return nullptr;
	}

	if (EVP_DigestInit(context, sha256) != 1 ||
		EVP_DigestUpdate(context, password, strlen(password)) != 1 ||
		EVP_DigestFinal(context, hash, &lengthOfHash) != 1
		) 
	{
		EVP_MD_CTX_free(context);
		
	}

	return hash;
}
