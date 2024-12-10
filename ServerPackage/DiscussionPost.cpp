#include "DiscussionPost.h"
#include <string>



DiscussionPost::DiscussionPost(char* buffer, User& user, unsigned int headerSize)
{
	CreateId();
	unsigned int startIdx = 0;
	memcpy_s(_username, strlen(user.Username()), user.Username(), strlen(user.Username()));

	char* data = GetDataFromBuffer('\0', buffer, headerSize, startIdx);
	const size_t dataLength = strlen(data) + 1;

	memcpy_s(_post, dataLength, data, dataLength);
	_post[dataLength] = '\0';
	delete[] data;
}

void DiscussionPost::SetUsername(char* buffer)
{
	const size_t bufferSize = strlen(buffer);
	memcpy_s(_username, bufferSize + 1, buffer, bufferSize);

}

void DiscussionPost::SetPost(char* buffer)
{
	const size_t bufferSize = strlen(buffer);
	memcpy_s(_post, bufferSize + 1, buffer, bufferSize);
}

void DiscussionPost::CreateId()
{
	srand(time_t());

	_id = rand() % 3000000;


}

void DiscussionPost::SetAuthor(char* buffer, const unsigned int bufferOffset)
{
	memcpy_s(&_authorId, sizeOfInt, buffer + bufferOffset, sizeOfInt);
}

char* DiscussionPost::GetDataFromBuffer(char delimiter, char* buffer, const unsigned int expectedSize, unsigned int& startIdx)
{
	const size_t sizeOfBuffer = strlen(buffer);
	char* data = new char[sizeOfBuffer + 1];
	memcpy_s(data, sizeOfBuffer, buffer, sizeOfBuffer);
	
	data[sizeOfBuffer] = '\0';
	return data;
}






