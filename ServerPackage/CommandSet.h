#pragma once
#include "Constants.h"
#include "User.h"
#include "DiscussionPost.h"
class CommandSet
{
public:
	CommandSet();
	CommandSet(User* user);
	User LoginUser(const char * buffer, bool &success);
	User NewUser(const char* buffer, bool& success);
	DiscussionPost NewDiscussionPost( char* buffer, User& user, unsigned int headerSize);
};

