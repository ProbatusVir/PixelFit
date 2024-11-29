#include "FileOps.h"
#include "TokenHelper.h"

#include <fstream>
#include <filesystem>
#include <iostream>

FileOps::FileOps() { m_environment = nullptr; LoadEnvironment(); }
FileOps* FileOps::m_instance = new FileOps();

const char* FileOps::FetchEnvironmentVariable(const char* variable)
{
    Tokenizer tokens(m_environment);
    unsigned int fields = tokens.Fields();

    //Find the token that starts with the string you're looking for. If it's "DSN" and there is "DSN=DB", return "DB".
    char** seeker = tokens.Tokens();
    for (; seeker < tokens + fields; seeker++)
    {
        Tokenizer split(*seeker, 2, '=');
        if (!strcmp(split[0], variable))
        {
            unsigned int token_length = strlen(split[1]);
            char* value = new char[token_length + 1];
            value[token_length] = '\0';
            memcpy_s(value, token_length, split[1], token_length);
            
            return value;
        }
    }

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


