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
	char email[emailSize] = { 0 };
	char password[hashSize] = { 0 };

	//Find the length of the three fields
	const char* seeker = buffer;
	const char name_length = strpbrk(seeker, "\n") - seeker;
	const char username_length = strpbrk(seeker += name_length + 1, "\n") - seeker;
	const char email_length = strpbrk(seeker += username_length + 1, "\n") - seeker;
	const char password_length = strlen(seeker += email_length + 1);
	
	//Copy the three fields to their buffers
	seeker = buffer;
	memcpy_s(name,		name_length + 1,		seeker,							name_length);
	memcpy_s(username,	username_length + 1,	seeker += name_length + 1,		username_length);
	memcpy_s(email,		email_length + 1,		seeker += username_length + 1,	email_length);
	memcpy_s(password,	password_length + 1,	seeker += email_length + 1,		password_length);


	if (name_length + username_length + password_length + email_length == strlen(buffer) - 3) { //Verify the size of the fields

		char* hashed = User::HashPassword(password);
		// This tells us that we are or are not able to make a database entry. If we cannot, then we tell the user.
		bool ableToCreateNewUser = SQLInterface::Instance()->InsertNewUser(name, username, email, hashed);
		if (ableToCreateNewUser) {
			char transferToCharHash[passwordSize] = { 0 };
			memcpy_s(transferToCharHash, strlen((char*)hashed), hashed, strlen((char*)hashed));

			//TODO: This is wrong.
			User newUser = User(name, username, transferToCharHash, 1);
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
