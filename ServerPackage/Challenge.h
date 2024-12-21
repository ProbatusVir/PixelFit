#pragma once

class ChallengeFactory {
public:

};

struct Challenge
{
	char* name;
	int roomID;
	char* description;
	char* punishment;
	char* condition;
	Challenge(const char* _punishment, const char* _condition) : punishment(nullptr), condition(nullptr) {};
	Challenge(const char* buffer, size_t buffer_size, const char*& reader) : name(nullptr), roomID(-1), punishment(nullptr), condition(nullptr) { buffer_size; };
	Challenge() : name(nullptr), roomID(-1), punishment(nullptr), condition(nullptr), description(nullptr) {};
	virtual void abstract()=0;
};

struct DurationChallenge : public Challenge
{
	unsigned short int time_in_seconds;
	DurationChallenge(const char* punishment, const char* condition, unsigned short int _time_in_seconds) : Challenge(punishment, condition), time_in_seconds(_time_in_seconds) {}
	DurationChallenge(const char* buffer, size_t buffer_size, const char*& reader);
};

struct RepChallenge : public Challenge
{
	unsigned short int reps;
	unsigned short int weight; //0 is body. -1 is choice.
	RepChallenge(const char* punishment, const char* condition, unsigned short int _reps, unsigned short int _weight) : Challenge(punishment, condition), reps(_reps), weight(_weight) {}
	RepChallenge(const char* buffer, size_t buffer_size, const char*& reader);
};