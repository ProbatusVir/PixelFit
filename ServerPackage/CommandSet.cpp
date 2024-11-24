#include "CommandSet.h"
#include <iostream>
#include "SQLInterface.h"
CommandSet::CommandSet()
{
}

CommandSet::CommandSet(User* user)
{

}



User CommandSet::LoginUser(const char* buffer, bool& success)
{
	constexpr size_t expectedBufferSize = usernameSize + passwordSize;

	char username[usernameSize] = { 0 };
	char password[passwordSize] = { 0 };

	bool fullReadOfUsername = false;
	bool error = false;
	User attemptLogin;
	size_t buff_seeker = 0;
	size_t sizeOfUsername = 0;

	//seek until End of Stream at null terminator, or until expected buffer size, to prevent fatal stack corruption
	while ((char)buffer[buff_seeker] != '\0' || buff_seeker >= expectedBufferSize) {
		// If the username hasn't been read, add the next character or terminate the username read 
		if (!fullReadOfUsername) {

			if (buffer[buff_seeker] != '\n') {
				username[buff_seeker] = buffer[buff_seeker];
			}
			else {
				fullReadOfUsername = true;
			}

			//Offset needs to be applied regardless here so we don't double count the newline
			sizeOfUsername++;

		}

		//Fill password buffer with whatever remains until null terminator
		else {
			password[buff_seeker - sizeOfUsername] = buffer[buff_seeker];
		}

		buff_seeker++;
	}
	char* checkPass = User::HashPassword(password);


	error = !(strlen((char*)checkPass) <= passwordSize) && (strlen(username) <= usernameSize);

	if (!error) {
		int comparedPass = -1;
		//TODO: When SQL calls can be made for our user objects, we need to contact the db
		// to get the user object. We then need to compare the hashed passwords against eachother
		// if they match, then allow the user login, otherwise prevent it.



		if (SQLInterface::Instance()->LoginRequest(username, checkPass)) {
			success = true;
			User user(username);
			return user;
		}
		else success = false;
		delete[] checkPass;
		return User();

	}

	return User();

}

User CommandSet::NewUser(const char* buffer, bool& success)
{
	char name[nameSize] = { 0 };
	char username[usernameSize] = { 0 };
	char password[hashSize] = { 0 };
	bool fullReadOnName = false;
	bool fullReadOnUsername = false;
	bool fullReadOnPassword = false;
	size_t sizeOfName = 0;
	size_t sizeOfUsername = 0;


	size_t buffSeeker = 0;

	while (buffer[buffSeeker] != '\0' && buffSeeker <= strlen(buffer)) {
		if (!fullReadOnName) {
			sizeOfName++;
			if (buffer[buffSeeker] != '\n')
				name[buffSeeker] = buffer[buffSeeker];

			else fullReadOnName = true;

		}
		else if (!fullReadOnUsername) {
			if (buffer[buffSeeker] != '\n')
				username[buffSeeker - sizeOfName] = buffer[buffSeeker];
			else fullReadOnUsername = true;
			sizeOfUsername++;
		}
		else {
			password[buffSeeker - sizeOfUsername - sizeOfName] = buffer[buffSeeker];
		}

		buffSeeker++;

	}
	int verifyReadName = strlen(name);
	int verifyReadUsername = strlen(username);
	int verifyReadPassword = strlen(password);

	if (verifyReadName && verifyReadUsername && verifyReadPassword != 0) {

		char* hashed = User::HashPassword(password);
		uint64_t id = CreateID();
		// This tells us that we are or are not able to make a database entry. If we cannot, then we tell the user.
		bool ableToCreateNewUser = SQLInterface::Instance()->InsertNewUser(name, username, hashed, id);
		if (ableToCreateNewUser) {
			char transferToCharHash[passwordSize] = { 0 };
			memcpy_s(transferToCharHash, strlen((char*)hashed), hashed, strlen((char*)hashed));

			User newUser = User(name, username, transferToCharHash, id);
			success = true;
			return newUser;
		}
		else {
			success = false;
			return User();
		}
	}
	else {
		success = false;
		return User();
	}

}
// This will handle the creation of discussion posts
DiscussionPost CommandSet::NewDiscussionPost(char* buffer, User& user, unsigned int headerSize)
{
	DiscussionPost info = DiscussionPost(buffer, user, headerSize);

	return info;
}

uint64_t CommandSet::CreateID()
{
	idIncrement = 0;
	idIncrement++;
	return idIncrement;
}
