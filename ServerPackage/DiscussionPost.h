#pragma once
#include "Constants.h"
#include "User.h"
class DiscussionPost
{
public:

	DiscussionPost(char* buffer, User& user, unsigned int headerSize);
	char* GetAuthor() { return _username; };
	char* GetPost() { return _post; };
	uint64_t Id() { return _id; };

private:
	void SetUsername(char* buffer);
	void SetPost(char* buffer);
	void CreateId();
	void SetAuthor(char* buffer, const unsigned int bufferOffset);
	char* GetDataFromBuffer(char delimiter, char* buffer, const unsigned int expectedSize,  unsigned int& startIdx);
	
	char _username[USERNAME_SIZE] = { 0 };
	char _post[sizeOfDiscussionPost] = { 0 };
	uint64_t _id;
	uint64_t _authorId;
	
	
};

