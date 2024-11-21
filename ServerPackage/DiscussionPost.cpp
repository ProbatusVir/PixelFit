#include "DiscussionPost.h"
#include <string>



DiscussionPost::DiscussionPost(char* buffer, User& user, unsigned int headerSize)
{
	CreateId();
	unsigned int startIdx = 0;
	memcpy_s(_username, strlen(user.Username()), user.Username(), strlen(user.Username()));

	char* data = GetDataFromBuffer('\0', buffer, headerSize, startIdx);
	unsigned int dataLength = strlen(data) + 1;

	memcpy_s(_post, dataLength, data, dataLength);
	_post[dataLength] = '\0';
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
	unsigned int sizeOfBuffer = strlen(buffer);
	char* data = new char[sizeOfBuffer + 1];
	memcpy_s(data, sizeOfBuffer, buffer, sizeOfBuffer);
	
	data[sizeOfBuffer] = '\0';
	return data;
}






