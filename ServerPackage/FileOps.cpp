#include "FileOps.h"
#include "TokenHelper.h"

#include <fstream>
#include <filesystem>
#include <iostream>

FileOps::FileOps()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
EnvironmentFile::EnvironmentFile() { m_environment = nullptr; LoadEnvironment(); }
EnvironmentFile* EnvironmentFile::m_instance = new EnvironmentFile();

const char* EnvironmentFile::FetchEnvironmentVariable(const char* variable)
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

void EnvironmentFile::LoadEnvironment()
{
    static constexpr const char* file_name = ".env";
    std::ifstream envFile(file_name);

    if (!envFile)
    {
        std::cout << "Error: encountered trouble opening .env file, make sure it exists.";
        return;
    }

    const unsigned int file_size = std::filesystem::file_size(file_name);

    m_environment = new char[file_size];
    m_environment[file_size] = '\0';

    envFile.read(m_environment, file_size);
    envFile.close();
}

