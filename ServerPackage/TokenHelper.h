
#include <string.h>
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
	Tokenizer(const char* message, const char delim = '\n');

	char** Tokens() { return m_container; }

	char* operator[](unsigned int n) { return (n >= 0 && n < m_fields) ? m_container[n] : nullptr;}

	operator char** () { return m_container; }

	unsigned int Fields() {return m_fields;}

	~Tokenizer() { DestroyTokens(); }

private:

	char** m_container = nullptr;
	unsigned int m_fields = 0;

	void Initialize(const char* message, const char delim = '\n');

	void DestroyTokens();
};
