#include "CommandSet.h"

int CommandSet::InterpretRequest(int command, char* buffer)
{
	// zero initializes and we return zero it will be a failed attempt for the server to call back to.
	int requestComplete = 0;
	switch (command) {
	case 1:
		if (LoginUser(buffer)) {
			requestComplete = 1;
		}
		break;
	case 2:

		break;

	default:

		return requestComplete;
	}

	return requestComplete;
}

bool CommandSet::LoginUser(char* buffer)
{
	char username[30] = { 0 };
	char password[60] = { 0 };

	bool fullReadOfUsername = false;

	int counter = 0;
	int offset = 0;
	while ((char)counter != '\0') {
		if (!fullReadOfUsername) {

			if (buffer[counter] != '\n') {
				username[counter] = buffer[counter];

				offset++;
			}
			else {
				fullReadOfUsername = true;


			}

		}
		else {
			password[counter - offset] = buffer[counter];
		}
		counter++;
	}





	return false;
}
