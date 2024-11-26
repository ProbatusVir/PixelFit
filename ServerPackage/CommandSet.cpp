#include "CommandSet.h"
#include "TokenHelper.h"
#include "SQLInterface.h"

#include <iostream>


CommandSet::CommandSet()
{
}

CommandSet::CommandSet(User* user)
{

}


User CommandSet::LoginUser(const char* buffer, bool& success)
{
	success = false;

	constexpr const unsigned int fields = 2;
	char** tokens = Tokenize(buffer, fields);

	const char* username = tokens[0];
	const char* password = tokens[1];

	char* checkPass = User::HashPassword(password);

	const bool error = !(strlen((char*)checkPass) <= passwordSize) && (strlen(username) <= usernameSize);

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
	success = false;
	static constexpr const unsigned int fields = 4;
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
	const bool dataNonZero = name_length && username_length && email_length && password_length;
	const bool dataParsedProperly = name_length + username_length + email_length + password_length == strlen(buffer) - (fields - 1);
	
	if (!dataFits || !dataParsedProperly || !dataNonZero)
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
		return User(username);
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
