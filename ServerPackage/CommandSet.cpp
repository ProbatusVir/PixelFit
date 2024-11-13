#include "CommandSet.h"
#include <iostream>

CommandSet::CommandSet()
{
}

CommandSet::CommandSet(User* user)
{
	
}

int CommandSet::InterpretRequest(const Command command, const char* buffer, User* user)
{
	// zero initializes and we return zero it will be a failed attempt for the server to call back to.
	int requestComplete = 0;
	bool loginAttempt = false;
	
	switch (command) {
	case Command::Login:
		std::cout << buffer << '\n';
		user = LoginUser(buffer, loginAttempt);
		if (loginAttempt) {
			requestComplete = 1;
			std::cout << "Success\n";
			
		}
		
		else {
			requestComplete = 0;
			user = nullptr;
			std::cout << "Failed\n";
		}
		break;
	case Command::GetUsers:
		std::cout << buffer << '\n';
		break;

	case Command::MessageServer:

		std::cout << buffer << '\n';

		break;

	default:

		return requestComplete;
		break;
	}

	return requestComplete;
}

User* CommandSet::LoginUser(const char* buffer, bool& success)
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
	unsigned char* checkPass = attemptLogin.HashPassword(password);


	error = !(strlen((char*)checkPass) <= passwordSize) && (strlen(username) <= usernameSize);

	if (!error) {
		int comparedPass = -1;
		//TODO: When SQL calls can be made for our user objects, we need to contact the db
		// to get the user object. We then need to compare the hashed passwords against eachother
		// if they match, then allow the user login, otherwise prevent it.
		unsigned char* removeAfterDebug = attemptLogin.HashPassword("abcdef");
		comparedPass = strcmp((char*)checkPass, (char*)removeAfterDebug);
		if (comparedPass == 0) success = true;
		else success = false;
		delete[] checkPass;
		delete[] removeAfterDebug;
		// TODO: remove name after SQL integration
		char name[20] = "someone";
		uint64_t id = CreateID();
		User* newUser = new User(name, username, password, id);
		return newUser;
	}
	else return nullptr;


}

uint64_t CommandSet::CreateID()
{
	idIncrement = 0;
	idIncrement++;
	return idIncrement;
}
