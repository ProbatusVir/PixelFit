#pragma once
#include "Challenge.h"
#include "vector"
#include <unordered_map>

//This is a crime
struct Challenges
{
	static std::unordered_map<int, Challenge*> challenges;
};

