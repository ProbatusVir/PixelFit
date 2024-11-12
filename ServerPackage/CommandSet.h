#pragma once
#include "Constants.h"
class CommandSet
{
public:
	enum Command;

	int InterpretRequest(const int commmand, const char* buffer);
	bool LoginUser(const char * buffer);

private:
	

};

