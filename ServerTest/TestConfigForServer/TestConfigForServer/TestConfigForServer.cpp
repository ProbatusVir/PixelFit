// TestConfigForServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "ServerConnect.h"
#include <string>
#include "../../../ServerPackage/Constants.h"

constexpr const char* options[] =
{
    "1) Login user",
    "2) New user",
    "3) Message To Server",
    "4) New Discussion Post",
    "5) End",
};

void PrintMenu() {
    for (const char* option : options) {
        std::cout << option << '\n';
    }
}

void RequestNewPost(ServerConnect &server, unsigned char* token) {
    std::string username = "";
    std::string post = "";
    std::cout << "What is your username \n";
    std::getline(std::cin.ignore(), username);
    std::cout << "What is your post \n";
    std::getline(std::cin.ignore(), post);
    username += '\n';
    post += '\0';
    const int messageSize = username.size() + post.size();
    char* message = new char[messageSize + 1];
    memcpy_s(message, strlen(username.c_str()), username.c_str(), strlen(username.c_str()));
    memcpy_s(message, strlen(post.c_str()), post.c_str(), strlen(post.c_str()));
    server.SendToServer((int)Command::NewDiscussionPost, message);
    delete[] message;
}

void CreateNewUser(ServerConnect &server, unsigned char* token) {
    std::string name = "";
    std::string username = "";
    std::string password = "";
   
    std::cout << "Please input your name \n";
    std::getline(std::cin.ignore(), name);
    std::cout << "Please enter your username\n";
    std::getline(std::cin.ignore(), username);
    std::cout << "Please enter your password\n";
    std::getline(std::cin.ignore(), password);
    name += '\n';
    name += username;
    name += '\n';
    name += password;

    name += '\0';
    char* messageToServer = new char[name.size() + 1];
    memcpy_s(messageToServer, name.size(), name.c_str(), name.size());

    server.SendToServer((int)Command::NewUser, messageToServer);

    delete[] messageToServer;

}


void LoginInfo(ServerConnect &server, unsigned char* token) {

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
   char* messageToServer = new char[lengthOfMessage + 1];

   memcpy_s(messageToServer, username.size(), username.c_str(), username.size());
   messageToServer[lengthOfMessage] = '\0';
   server.SendToServer((int)Command::Login, messageToServer);

   delete[] messageToServer;
}

void SendMessageToServer(ServerConnect& server, unsigned char* token) {

    char message[1024];
    std::string response;
    std::cout << "Please enter your message to the server";
    std::getline(std::cin.ignore(), response);


    strcpy_s(message, response.c_str());
    
   
    server.SendToServer((int) Command::MessageServer, message);

}

int main()
{
    ServerConnect server;
    bool keepAlive = true;
    unsigned char* token = nullptr;
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

        case 5:
            keepAlive = false;
            break;

        default:
            std::cout << "Wrong input \n";
            break;
        }
        


    }
    if (token != nullptr) delete[] token;
    
}
