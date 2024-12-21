#include "Challenge.h"
#include "Header.h"

//
// name -> const char*
// description -> const char*
// roomID -> int
// condition -> const char*
// punishment -> Punishment??? -> string???
// author???

DurationChallenge::DurationChallenge(const char* buffer, size_t buffer_size, const char*& reader)
{
	// Copy name
	size_t name_len = strlen(reader); 
	name = new char[name_len + 1]; name[name_len] = '\0';
	memcpy(name, reader, name_len);
	reader += name_len;

	size_t description_len = strlen(reader);
	description = new char[description_len + 1]; description[description_len] = '\0';
	memcpy(description, reader, description_len);
	reader += description_len;

	memcpy(&roomID, reader, sizeof(roomID));
	reader += sizeof(roomID);

	size_t condition_len = strlen(reader);
	condition = new char[condition_len + 1]; condition[condition_len] = '\0';
	memcpy(condition, reader, condition_len);
	reader += condition_len;

	//Figure this out.
	size_t punishment_len = strlen(reader);
	punishment = new char[punishment_len + 1]; punishment[punishment_len] = '\0';
	memcpy(punishment, reader, punishment_len);
	reader += punishment_len;



}

//RepChallenge::RepChallenge(const char* buffer, size_t buffer_size, const char& reader)
//{
//}
