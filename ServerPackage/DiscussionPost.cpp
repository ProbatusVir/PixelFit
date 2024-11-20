#include "DiscussionPost.h"
#include <string>



DiscussionPost::DiscussionPost(char* buffer, User& user)
{
	CreateId();
	unsigned int startIdx = 0;
	memcpy_s(_username, strlen(user.Username()), user.Username(), strlen(user.Username()));

	char* data = GetDataFromBuffer('\n', buffer, sizeOfDiscussionPost, startIdx);
	memcpy_s(_post, sizeOfDiscussionPost, data, sizeOfDiscussionPost);
	delete[] data;
}

void DiscussionPost::SetUsername(char* buffer)
{
	unsigned int bufferSize = strlen(buffer);
	bufferSize++;
	memcpy_s(_username, bufferSize, buffer, bufferSize - 1);

}

void DiscussionPost::SetPost(char* buffer)
{
	unsigned int bufferSize = strlen(buffer);
	bufferSize++;
	memcpy_s(_post, bufferSize, buffer, bufferSize - 1);
}

void DiscussionPost::CreateId()
{
	srand(time_t());

	_id = rand() % 3000000;


}

void DiscussionPost::SetAuthor(char* buffer, const unsigned int bufferOffset)
{
	unsigned int authorId = 0;
	memcpy_s(&authorId, sizeOfInt, buffer + bufferOffset, sizeOfInt);
	_authorId = authorId;
}

char* DiscussionPost::GetDataFromBuffer(char delimiter, char* buffer, const unsigned int expectedSize, unsigned int& startIdx)
{
	char* data = new char[expectedSize + 1];
	unsigned int idx = startIdx;
	unsigned int writeToIdx = 0;
	while (buffer[idx] != delimiter) {
		data[writeToIdx] = buffer[idx];
	}

	return data;
}






