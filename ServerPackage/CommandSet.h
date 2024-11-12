#pragma once
class CommandSet
{
public:
	enum Command;

	int InterpretRequest(const Command commmand, const char* buffer);
	bool LoginUser(const char * buffer);

private:
	

};

