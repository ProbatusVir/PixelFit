
#include <string.h>
#include <algorithm>
//TODO: Make this into a real class, so we don't put memory management on the other end.
	//Parses tokens!
	static char** Tokenize(const char* message, const unsigned int fields, const char delim = '\n')
	{
		const char delimiter[] = { delim, '\0' };
		char** container = new char* [fields];
		const char* seeker = message;
		unsigned char token_length = 0;

		for (unsigned int i = 0; i < fields; i++)
		{
			if (i < fields - 1)
				token_length = strpbrk(seeker, delimiter) - seeker;
			else
				token_length = strlen(seeker);

			container[i] = new char[token_length + 1];

			memcpy_s(container[i], token_length + 1, seeker, token_length);
			container[i][token_length] = '\0';

			seeker += token_length + 1;
		}

		return container;
	}

	//Not sure how many fields there are? No problem.
	static char** Tokenize(const char* message, unsigned int* _fields, const char delim = '\n')
	{
		//Determine number of fields
		const unsigned int message_length = strlen(message);
		unsigned int fields = 1;

		for (int i = 0; i < message_length; i++)
			fields += message[i] == delim;

		*_fields = fields;
		return Tokenize(message, fields, delim);
	}

	static void DestroyTokens(char** container, const unsigned int fields)
	{
		for (int i = 0; i < fields; i++)
			delete[] container[i];

		delete[] container;
	}

