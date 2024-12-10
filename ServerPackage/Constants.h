#pragma once
#include <openssl/sha.h>


	enum class Command : int
	{
		Login = 1,
		NewUser,
		GetUsers,
		MessageServer,
		NewDiscussionPost,
		DiscussionPost,
		GetUser,
		BanUser,
		SendImageToServer,
		LogOut,
		RequestData,
	};

	enum class ResourceType : int {
		PNG = 1196314761 //This makes MUCH more sense on the Kotlin side, I swear.
	};

	enum class MessageResult : int {
		Failed,
		LoginSuccess,
		Success
	};

	// use port numbers from 3500 - 65000. 
	// Avoid using ports lower than 3500 due to IATA protocols
	
	//remotehost must change per-run
	static constexpr const char REMOTEHOST[] = "3.22.30.40";//tcp.ngrok.io
	//These never change
	static constexpr const char LOCALHOST[] = "127.0.0.1";
	static constexpr int PORT = 5930;

	static constexpr size_t PACKET_SIZE = 1024;
	static constexpr size_t HASH_SIZE = SHA256_DIGEST_LENGTH;
	static constexpr size_t NAME_SIZE = 50;
	static constexpr size_t USERNAME_SIZE = 30;
	static constexpr size_t EMAIL_SIZE = 60;
	static constexpr size_t PASSWORD_SIZE = 60;
	//I'm a little unsure of how to approach these two...
	static constexpr unsigned int sizeOfInt = 4;
	static constexpr unsigned int sizeOfDiscussionPost = 1024;


