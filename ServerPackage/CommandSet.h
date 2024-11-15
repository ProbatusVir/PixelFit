#pragma once
#include "Constants.h"
#include "User.h"

#include <vector>
class CommandSet
{
public:
	CommandSet();
	CommandSet(User* user);
	User LoginUser(const char * buffer, bool &success);
	User NewUser(const char* buffer, bool& success);

private:
	//TODO: Add logic to save the next available id and load to the idIncrmenter
	 uint64_t CreateID();
	 uint64_t idIncrement;
	 

};

