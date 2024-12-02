#include "FileOps.h"
#include "TokenHelper.h"

#include <fstream>
#include <filesystem>
#include <iostream>


unsigned int FileOps::GetFileSize(const char* file_name) const
{
	try
	{
		return std::filesystem::file_size(file_name);
	}
	catch (...)
	{
		return 0;
	}
}

/// <summary>
/// Read a full file into the FileOps instance's buffer. The size needn't be known.
/// </summary>
/// <param name="file_name:"> The name of the file</param>
/// <param name="file_size:"> The size of the file, if set to 0 will populate with the file size according to the OS</param>
/// <param name="null_terminate:"> Whether or not the buffer should be null terminated</param>
/// <returns>A pointer to the FileOps internal buffer.</returns>
const char* const FileOps::ReadFullFile(const char* file_name, bool null_terminate)
{
	const unsigned int file_size = GetFileSize(file_name);

	m_file_size = file_size;

	m_file_buffer = new char[file_size + null_terminate];
	if (null_terminate) m_file_buffer[file_size] = '\0';

	WriteFileToBuffer(file_name, m_file_buffer, file_size);

	return m_file_buffer;
}

//If the buffer already exists, its size is already known.
void FileOps::WriteFileToBuffer(const char* file_name, char* buffer, const unsigned int bytes_to_read) const
{
	if (!bytes_to_read) { std::cerr << "There were no bytes to read..."; }

	std::ifstream file(file_name, std::ios::binary);
	if (!file) { std::cerr << "Something went wrong opening the file. Check directory.\n"; };

	file.read(buffer, bytes_to_read);
	file.close();
}

/// <summary>
/// Write a file
/// </summary>
/// <param name="file_name:"> Name of the file to read</param>
/// <param name="file_size:"> Variable is overwritten to provide the file size to the caller</param>
/// <returns>The file buffer</returns>

void FileOps::WriteFile(const char* file_name, const char* buffer, const unsigned int buffer_size)
{
	m_file_buffer = new char[buffer_size];

	// Verify file
	if (!buffer_size) { std::cerr << "There is nothing to write.\n";  return; }
	std::ofstream file = std::ofstream(file_name, std::ios::binary);
	if (!file) { std::cerr << "Error opening file.\n"; return; }

	//Write to file
	file.write(m_file_buffer, buffer_size);
	file.close();
}

void FileOps::BufferedWrite(const char* buffer, unsigned int buffer_size)
{
	//Copy into new buffer
	char* new_buffer = new char[m_file_size + buffer_size];
	memcpy_s(new_buffer, m_file_size, m_file_buffer, m_file_size);
	memcpy_s(new_buffer + m_file_size, buffer_size, buffer, buffer_size);

	m_file_size += buffer_size;

	//Reassign internal buffer
	delete[] m_file_buffer;
	m_file_buffer = new_buffer;
}

/// <summary>
/// Writes out the internal buffer.
/// </summary>
/// <param name="file_name:"> The file to write the internal buffer to.</param>
void FileOps::WriteFile(const char* file_name)
{
	if (m_file_buffer)
		WriteFile(file_name, m_file_buffer, m_file_size);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EnvironmentFile::EnvironmentFile() { m_file_buffer = nullptr; LoadEnvironment(); }
EnvironmentFile* EnvironmentFile::m_instance = nullptr;

/// <summary>
/// Fetches a variable from the .env file.
/// </summary>
/// <param name="variable:"> The name of the variable to fetch. </param>
/// <returns>The str value of that variable.</returns>
const char* EnvironmentFile::FetchEnvironmentVariable(const char* variable)
{
	Tokenizer tokens(m_file_buffer);
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

	ReadFullFile(file_name, true);
	

	if (!m_file_buffer) { std::cout << "Error: encountered trouble opening .env file, make sure it exists."; return; }
}