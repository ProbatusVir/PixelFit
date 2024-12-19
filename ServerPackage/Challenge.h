#pragma once
struct Challenge
{
	const char* punishment;
	const char* condition;
	Challenge(const char* _punishment, const char* _condition) : punishment(_punishment), condition(_condition) {};
};

struct DurationChallenge : public Challenge
{
	unsigned short int time_in_seconds;
	DurationChallenge(const char* punishment, const char* condition, unsigned short int _time_in_seconds) : Challenge(punishment, condition), time_in_seconds(_time_in_seconds) {}
};

struct RepChallenge : public Challenge
{
	unsigned short int reps;
	unsigned short int weight; //0 is body. -1 is choice.
	RepChallenge(const char* punishment, const char* condition, unsigned short int _reps, unsigned short int _weight) : Challenge(punishment, condition), reps(_reps), weight(_weight) {}
};