#pragma once
#include "Constants.h"
class CommandSet
{
public:
	int InterpretRequest(const Command commmand, const char* buffer);
	bool LoginUser(const char * buffer);

private:
	

};

