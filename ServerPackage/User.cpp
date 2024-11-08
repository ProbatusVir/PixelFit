#include "User.h"
#include <string>




User::User(char name[50], char userName[20], char password[60])
{
	strcpy_s(_name, name);
	strcpy_s(_userName, userName);
	unsigned char* hashed = HashPassword(password);

	if (hashed == nullptr) _errorOnCreation = true;
	else {

		
		memcpy_s(_password, EVP_MAX_MD_SIZE, hashed, EVP_MAX_MD_SIZE);
	}


}

User::User()
{
}

User::~User()
{
	
}

unsigned char* User::HashPassword(char password[60])
{
	unsigned char hash[SHA256_DIGEST_LENGTH] = { 0 };
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
