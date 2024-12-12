#pragma once
#include <openssl/sha.h>
#include <algorithm>


consteval int rstrtoint(const char str[sizeof(int)]) { int result = 0; for (int i = 0; i < sizeof(result); i++) result += str[i] * (1 << (8 * i)); return result; }
consteval int strtoint(const char str[sizeof(int)]) { int result = 0; for (int i = 0; i < sizeof(result); i++) result += str[i] * (1 << (8 * ((sizeof(result) - 1) - i))); return result; }

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

	//No matter what, I guess because of how hex literals work, there is no elegant way of doing this. So these have to be opposite endian of the Kotlin client
	enum class ResourceType : int {
		PNG = rstrtoint("PNG"),
		DIR = rstrtoint("DIR"),
		
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


