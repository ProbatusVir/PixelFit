// TestConfigForServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "ServerConnect.h"
#include <string>
#include "../../../ServerPackage/Constants.h"
#define SIZE_OF_MENU 4

std::string* MakeMenu() {

    std::string* options = new std::string[SIZE_OF_MENU];
    options[0] = "1) Login user";
    options[1] = "2) New user";
    options[2] = "3) Message To Server";
    options[3] = "4) End";

    return options;
}

void PrintMenu(std::string* options, int size) {
    for (int i = 0; i < size; i++) {
        std::cout << options[i] << '\n';
    }
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
    std::string* options = MakeMenu();
    bool keepAlive = true;
    unsigned char* token = nullptr;
    while (keepAlive) {
        int response = 0;
        PrintMenu(options, SIZE_OF_MENU);
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
            keepAlive = false;
            break;

        default:
            std::cout << "Wrong input \n";
            break;
        }
        


    }
    if (token != nullptr) delete[] token;
    delete[] options;
    
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
