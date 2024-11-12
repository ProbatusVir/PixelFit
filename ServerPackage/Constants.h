#pragma once
#include <openssl/sha.h>


	enum Command
	{
		Login = 1, GetUsers,
		DiscussionPost,
		GetUser, BanUser,
	};

	static constexpr size_t hashSize = SHA256_DIGEST_LENGTH;
	static constexpr size_t nameSize = 50;
	static constexpr size_t usernameSize = 30;
	static constexpr size_t passwordSize = 60;


