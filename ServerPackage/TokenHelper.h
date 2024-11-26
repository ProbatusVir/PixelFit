
#include <string.h>
	//Parses '\n' delimited tokens. Might change.

	static char** Tokenize(const char* message, const unsigned int fields, const char* delimiter = "\n")
	{
		char** container = new char* [fields];
		const char* seeker = message;
		unsigned char token_length = 0;

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

	static void DestroyTokens(char** container, const unsigned int fields)
	{
		for (int i = 0; i < fields; i++)
			delete[] container[i];

		delete[] container;
	}

