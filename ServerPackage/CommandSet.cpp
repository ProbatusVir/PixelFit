#include "CommandSet.h"
#include <iostream>
#include "Constants.h"

int CommandSet::InterpretRequest(const Command command, const char* buffer)
{
	// zero initializes and we return zero it will be a failed attempt for the server to call back to.
	int requestComplete = 0;
	switch (command) {
	case Login:
		if (LoginUser(buffer)) {
			requestComplete = 1;
		}
		break;
	case GetUsers:	//TODO: Is this the right behavior?
		std::cout << buffer << '\n';
		break;

	default:

		return requestComplete;
		break;
	}

	return requestComplete;
}

bool CommandSet::LoginUser(const char* buffer)
{
	constexpr size_t expectedBufferSize = usernameSize + passwordSize;

	char username[usernameSize] = { 0 };
	char password[passwordSize] = { 0 };

	bool fullReadOfUsername = false;
	bool error = false;

	int buff_seeker = 0;
	int _usernameSize = 0;

	//seek until End of Stream at null terminator, or until expected buffer size, to prevent fatal stack corruption
	while ((char)buffer[buff_seeker] != '\0' || buff_seeker >= expectedBufferSize) {
		// If the username hasn't been read, add the next character or terminate the username read 
		if (!fullReadOfUsername) {

			if (buffer[buff_seeker] != '\n') {
				username[buff_seeker] = buffer[buff_seeker];
			}
			else {
				username[buff_seeker] = '\0'; //This string needs to be delimited.
				fullReadOfUsername = true;
			}

			//Offset needs to be applied regardless here so we don't double count the newline
			_usernameSize++;

		}

		//Fill password buffer with whatever remains until null terminator
		else {
			password[buff_seeker - _usernameSize] = buffer[buff_seeker];
		}

		buff_seeker++;
	}
	
	password[buff_seeker] = '\0';

	error = !(strlen(password) <= passwordSize) && (strlen(username) <= usernameSize);


	return error;
}
