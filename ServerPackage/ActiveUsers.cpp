#include "ActiveUsers.h"

ActiveUsers* ActiveUsers::instance = nullptr;

const User* ActiveUsers::FindUserByToken(const char* token) const
{
	const auto existingToken = _clientPairs.find(token);
	if (existingToken != _clientPairs.end())
		return &(existingToken->second.user);

	return nullptr;
}

const User* ActiveUsers::FindUserByToken(const std::string& token) const
{
	const auto existingToken = _clientPairs.find(token);
	if (existingToken != _clientPairs.end())
		return &(existingToken->second.user);

	return nullptr;
}

User* ActiveUsers::FindUserByToken(const char* token)
{
	const auto existingToken = _clientPairs.find(token);
	if (existingToken != _clientPairs.end())
		return &(existingToken->second.user);

	return nullptr;
}

User* ActiveUsers::FindUserByToken(const std::string& token)
{
	const auto existingToken = _clientPairs.find(token);
	if (existingToken != _clientPairs.end())
		return &(existingToken->second.user);

	return nullptr;
}

std::unordered_map<std::string, WindowsUserPair>& ActiveUsers::data()
{
	return _clientPairs;
}

bool ActiveUsers::VerifyUserAuth(const char* token) const
{
	return FindUserByToken(token);
}

bool ActiveUsers::EnsureSingleTokenInstance(const std::string& token) const //This can be simplified
{
	if (_clientPairs.find(token) == _clientPairs.end())
		return true;

	return false;
}