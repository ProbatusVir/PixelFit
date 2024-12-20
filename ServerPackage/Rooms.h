#pragma once
#include "ActiveUsers.h"
#include "WindowsUserPair.h"

typedef void* Challenge; //FIXME, do NOT forget

class Rooms
{
	
	static int instance;
};

class Room 
{
	//might have a time field here
	std::vector<WindowsUserPair*> user_list;
	std::vector<WindowsUserPair*> whitelist; //if this is empty, it's a free room
	const char* room_name;
	const char* description;
};

class ChallengeRoom : Room
{
	Challenge* challenge;
};
