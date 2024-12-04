#include "FileOps.h"
#include "TokenHelper.h"

#include <fstream>
#include <filesystem>
#include <iostream>

FileOps::FileOps() { m_environment = nullptr; LoadEnvironment(); }
FileOps* FileOps::m_instance = new FileOps();

const char* FileOps::FetchEnvironmentVariable(const char* variable)
{
    unsigned int* fields = new unsigned int;
    char** tokens = Tokenize(m_environment, fields);
    
    //Find the token that starts with the string you're looking for. If it's "DSN" and there is "DSN=DB", return "DB".
    char** seeker = tokens;
    for (; seeker < tokens + *fields; seeker++)
    {
        char** split = Tokenize(*seeker, 2, '=');
        if (!strcmp(split[0], variable))
        {
            unsigned int token_length = strlen(split[1]);
            char* value = new char[token_length + 1];
            value[token_length] = '\0';
            memcpy_s(value, token_length, split[1], token_length);
            
            DestroyTokens(tokens, *fields);
            delete fields;
            return value;
        }
    }

    DestroyTokens(tokens, *fields);
    delete fields;
    return nullptr;
}

void FileOps::LoadEnvironment()
{
    std::ifstream envFile(".env");

    if (!envFile)
    {
        std::cout << "Error: encountered trouble opening .env file, make sure it exists.";
        return;
    }

    const unsigned int file_size = std::filesystem::file_size(".env");

    m_environment = new char[file_size + 1];
    m_environment[file_size] = '\0';

    envFile.read(m_environment, file_size);
    envFile.close();
}


