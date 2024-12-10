#include "User.h"
#include <string>




User::User(const char userName[USERNAME_SIZE])
{
	strcpy_s(_userName, userName);
	CreateToken();
}

User::User()
{
}

User::User(const User& user)
{
	memcpy_s(_userName, sizeof(_userName), user._userName, sizeof(user._userName));

	CreateToken();
}

User::~User()
{

}

void User::RemakeToken()
{
	CreateToken();
}





 char* User::HashPassword(const char password[PASSWORD_SIZE])
{
	char* hash = new char[HASH_SIZE + 1] { 0 };
	EVP_MD_CTX* context = EVP_MD_CTX_new();
	const EVP_MD* sha256 = EVP_sha256();
	unsigned int lengthOfHash = 0;
	if (context == nullptr) {
		return nullptr;
	}

	if (EVP_DigestInit(context, sha256) != 1 ||
		EVP_DigestUpdate(context, password, strlen(password)) != 1 ||
		EVP_DigestFinal(context, (unsigned char*) hash, &lengthOfHash) != 1
		)
	{
		EVP_MD_CTX_free(context);

	}

	return hash;
}



void User::CreateToken()
{
	srand(static_cast<unsigned int>(time(nullptr)));
	unsigned char token[HASH_SIZE + 1] = { 0 };
	for (int i = 0; i < HASH_SIZE; i++) {
		unsigned int random = rand() % 255 + 33;
		token[i] = (char)random;

	}
	

	memcpy_s(_token, HASH_SIZE, token, HASH_SIZE);
	_token[HASH_SIZE] = '\0';

}
