
#include <string.h>
#include <algorithm>
class Tokenizer
{

//Parses tokens!
public:
	Tokenizer(const char* message, const unsigned int fields, const char delim = '\n') :
		m_fields(fields)
	{
		Initialize(message, delim);
	}

	//Not sure how many fields there are? No problem.
	Tokenizer(const char* message, const char delim = '\n')
	{
		//Determine number of fields
		const unsigned int message_length = strlen(message);
		m_fields = 1;

		for (int i = 0; i < message_length; i++)
			m_fields += message[i] == delim;

		Initialize(message, delim);
	}

	char** Tokens()
	{
		return m_container;
	}

	char* operator[](unsigned int n)
	{
		if (n >= 0 && n < m_fields)
			return m_container[n];
		
		return nullptr;
	}

	operator char** () { return m_container; }


	unsigned int Fields() {return m_fields;}

	~Tokenizer() { DestroyTokens(); }

private:

	char** m_container = nullptr;
	unsigned int m_fields = 0;

	void Initialize(const char* message, const char delim = '\n')
	{
		const char delimiter[] = { delim, '\0' };
		m_container = new char* [m_fields];
		const char* seeker = message;
		unsigned char token_length = 0;

		for (unsigned int i = 0; i < m_fields; i++)
		{
			if (i < m_fields - 1)
				token_length = strpbrk(seeker, delimiter) - seeker;
			else
				token_length = strlen(seeker);

			m_container[i] = new char[token_length + 1];

			memcpy_s(m_container[i], token_length + 1, seeker, token_length);
			m_container[i][token_length] = '\0';

			seeker += token_length + 1;
		}
	}

	void DestroyTokens()
	{
		for (int i = 0; i < m_fields; i++)
			delete[] m_container[i];

		if (m_container)
			delete[] m_container;
	}
};
