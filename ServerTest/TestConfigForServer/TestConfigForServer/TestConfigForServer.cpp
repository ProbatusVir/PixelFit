//Including the cpp files kinda sucks...
#include "ServerConnect.h"
#include "../../../ServerPackage/Constants.h"
#include "../../../ServerPackage/FileOps.cpp"
#include "../../../ServerPackage/TokenHelper.cpp"

#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>

static constexpr const char* options[] =
{
	"1) Login user",
	"2) New user",
	"3) Message To Server",
	"4) New Discussion Post",
	"5) Send Image to Server",
	"6) End",
};

void PrintMenu() {
	for (const char* option : options)
		std::cout << option << '\n';
}

std::string GetUserInput(const std::string& question) {
	std::string response = "";
	
	std::cout << question;
	std::cout << "\nInput: ";
	std::cin >> response;
	return response;
}

void RequestNewPost(ServerConnect& server, const unsigned char* token) {

	std::string post = "";
	std::cout << "What is your post \n";
	std::getline(std::cin.ignore(), post);

	post += '\0';
	const int messageSize = post.size();
	char* message = new char[messageSize + 1];

	memcpy_s(message, messageSize, post.c_str(), post.size());

	server.SendToServer((int)Command::NewDiscussionPost, message);
	delete[] message;
}

void CreateNewUser(ServerConnect& server, const unsigned char* token) {
	std::string name = "";
	std::string username = "";
	std::string email = "";
	std::string password = "";

	name = GetUserInput("What is your name");
	username = GetUserInput("\nWhat is your username?");
	email = GetUserInput("\nWhat is your email?");
	password = GetUserInput("\nWhat is your password?");
	name += '\n';
	name += username;
	name += '\n';
	name += email;
	name += '\n';
	name += password;
	name += '\0';

	char* messageToServer = new char[name.size() + 1];
	memcpy_s(messageToServer, name.size(), name.c_str(), name.size());

	server.SendToServer((int)Command::NewUser, messageToServer);

	delete[] messageToServer;

}


void LoginInfo(ServerConnect& server, const unsigned char* token) {

	std::string username = "";
	std::string password = "";
	std::string response;
	std::cout << "Please type in your username.\n";
	std::getline(std::cin.ignore(), response);
	username = response;
	response = "";
	std::cout << "Please type in your password.\n";
	std::getline(std::cin, response);
	password = response;
	username += '\n';
	username += password;
	username += '\0';
	unsigned int lengthOfMessage = username.size() + password.size();
	char* messageToServer = new char[lengthOfMessage + 1]; //This doesn't look right.

	memcpy_s(messageToServer, username.size(), username.c_str(), username.size());
	messageToServer[lengthOfMessage] = '\0';
	server.SendToServer((int)Command::Login, messageToServer);

	delete[] messageToServer;
}

void SendMessageToServer(ServerConnect& server, const unsigned char* token) {

	char message[1024];
	std::string response;
	std::cout << "Please enter your message to the server";
	std::getline(std::cin.ignore(), response);


	strcpy_s(message, response.c_str());


	server.SendToServer((int)Command::MessageServer, message);

}

void SendImageToServer(ServerConnect& server, const unsigned char* token)
{
	static constexpr unsigned int command = (unsigned int)Command::SendImageToServer;
	static constexpr unsigned int token_size = hashSize + 1;
	static constexpr unsigned int header_size = sizeof(unsigned int) * 3 + token_size; //cmd, tkSize, tk, fSize, f
	
	const FileOps* fop = new FileOps();

	if (!token)
	{
		std::cerr << "You don't have a token! Please make a token before attempting to send another image.\n"; return;
	}

	bool readError = false;
	std::string response;
	std::cout << "Please enter the filepath of your image: ";
	std::getline(std::cin.ignore(), response);
	const char* file_name = response.c_str();


	const unsigned int file_size = fop->GetFileSize(file_name);
	if (!file_size) { std::cerr << "Could not find the file requested: " << file_name << '\n'; return; }
	
	const size_t message_size = header_size + file_size;
	char* message = new char[message_size];
	char* write_point = message;
	memcpy_s(write_point,				sizeOfInt,	&command,		sizeOfInt);
	memcpy_s(write_point += sizeOfInt,	sizeOfInt,	&token_size,	sizeOfInt);
	memcpy_s(write_point += sizeOfInt,	token_size, token,			token_size);
	memcpy_s(write_point += token_size,	sizeOfInt,	&file_size,		sizeOfInt);
	FileOps().WriteFileToBuffer(file_name, write_point += sizeOfInt, file_size);

	server.SendToServerRaw(message, sizeOfInt * 2 + file_size);
	delete[] message;
}

int main()
{
	int selectLan = 1;
	std::cout << "Press 1 for LAN connection, 2 for localhost, 3 for remotehost\n";
	std::cin >> selectLan;
	ServerConnect server(selectLan);
	bool keepAlive = true;
	const unsigned char* token = (unsigned char*)server.Token();

	while (keepAlive) {
		int response = 0;
		PrintMenu();
		std::cout << "Please select a option \n";
		std::cin >> response;

		switch (response) {

		case 1:

			LoginInfo(server, token);

			break;

		case 2:
			CreateNewUser(server, token);
			break;

		case 3:

			SendMessageToServer(server, token);
			break;
		case 4:
			RequestNewPost(server, token);
			break;
		case 5: SendImageToServer(server, token); break;
		case 6:
			keepAlive = false;
			break;

		default:
			std::cout << "Wrong input \n";
			break;
		}



	}
}
