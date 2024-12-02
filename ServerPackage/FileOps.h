#pragma once

class FileOps
{
public:
	void BufferedWrite(const char* buffer, unsigned int buffer_size);
	void WriteFile(const char* file_name);
	void WriteFile(const char* file_name, const char* buffer, const unsigned int buffer_size);
	unsigned int GetFileSize(const char* file_name) const;
	const char* const ReadFullFile(const char* file_name, bool null_terminate = false);
	void WriteFileToBuffer(const char* file_name, char* buffer, const unsigned int bytes_to_read) const;
	const char* const GetBuffer() const { return m_file_buffer; };
	unsigned int FileSize() const { return m_file_size; };
	
	FileOps() : m_file_buffer(nullptr), m_file_size(0) {};
protected:
	char* m_file_buffer = nullptr;
	unsigned int m_file_size = 0;
};

class EnvironmentFile : protected FileOps
{
public:
	static EnvironmentFile* Instance() { if (!m_instance) m_instance = new EnvironmentFile();  return m_instance; };
	static EnvironmentFile* Destroy() { delete m_instance; };
	
	const char* FetchEnvironmentVariable(const char* variable);

private:
	static EnvironmentFile* m_instance;

	void LoadEnvironment();
	EnvironmentFile();
};