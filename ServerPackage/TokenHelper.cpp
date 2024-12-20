#include "TokenHelper.h"

Tokenizer::Tokenizer(const char* message, const char delim)
{
	//Determine number of fields
	const size_t message_length = strlen(message);
	m_fields = 1;

	for (size_t i = 0; i < message_length; i++)
		m_fields += message[i] == delim;

	Initialize(message, delim);
}

void Tokenizer::Initialize(const char* message, const char delim)
{
	const char delimiter[] = { delim, '\0' };
	m_container = new char* [m_fields];
	const char* seeker = message;
	size_t token_length = 0;

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

void Tokenizer::DestroyTokens()
{
	for (size_t i = 0; i < m_fields; i++)
		delete[] m_container[i];

	if (m_container)
		delete[] m_container;
}

std::string& trimstr(std::string& str)
{
	const size_t trail_start = str.find(' ');
	if (trail_start != std::string::npos)
		str.erase(trail_start);

	return str;
}

