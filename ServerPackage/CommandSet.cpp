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

//Parses '\n' delimited tokens. Might change.

char** Tokenize(const char* message, const unsigned int fields)
{
	char** container = new char* [fields];
	const char* seeker = message;
	char token_length = 0;

	for (unsigned int i = 0; i < fields; i++)
	{
		if (i < fields - 1)
			token_length = strpbrk(seeker, "\n") - seeker;
		else
			token_length = strlen(seeker);

		container[i] = new char[token_length + 1];

		memcpy_s(container[i], token_length + 1, seeker, token_length);
		container[i][token_length] = '\0';

		seeker += token_length + 1;
	}

	return container;
}

void DestroyTokens(char** container, const unsigned int fields)
{
	for (int i = 0; i < fields; i++)
		delete[] container[i];

	delete[] container;
}



User CommandSet::NewUser(const char* buffer, bool& success)
{
	success = false;
	static constexpr unsigned int fields = 4;
	char** tokens = Tokenize(buffer, fields);
	const char* name = tokens[0];
	const char* username = tokens[1];
	const char* email = tokens[2];
	const char* password = tokens[3];

	const int name_length = strlen(name);
	const int username_length = strlen(username);
	const int email_length = strlen(email);
	const int password_length = strlen(password);

	const bool dataFits = !(name_length > nameSize || username_length > usernameSize || email_length > emailSize || password_length > passwordSize);
	const bool dataParsedProperly = name_length + username_length + email_length + password_length == strlen(buffer) - (fields - 1);
	
	if (!dataFits || !dataParsedProperly)
	{
		DestroyTokens(tokens, fields);
		return User();
	}

	const char* hashed = User::HashPassword(password);
	// This tells us that we are or are not able to make a database entry. If we cannot, then we tell the user.
	const bool ableToCreateNewUser = SQLInterface::Instance()->InsertNewUser(name, username, email, hashed);
	if (ableToCreateNewUser) {
		DestroyTokens(tokens, fields);
		success = true;
		return User(name, username, hashed, 1);
	}
	
	DestroyTokens(tokens, fields);
	return User();
}
// This will handle the creation of discussion posts
DiscussionPost CommandSet::NewDiscussionPost(char* buffer, User& user, unsigned int headerSize)
{
	DiscussionPost info = DiscussionPost(buffer, user, headerSize);

	return info;
}
