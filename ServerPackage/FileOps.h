#pragma once

class FileOps
{
public:
	static size_t GetFileSize(const char* file_name);

	void BufferedWrite(const char* buffer, const size_t buffer_size);
	void WriteFile(const char* file_name);
	void WriteFile(const char* file_name, const char* buffer, const size_t buffer_size);
	const char* const ReadFullFile(const char* file_name, bool null_terminate = false);
	void WriteFileToBuffer(const char* file_name, char* buffer, const size_t bytes_to_read) const;
	const char* const GetBuffer() const { return m_file_buffer; };
	size_t FileSize() const { return m_file_size; };
	
	FileOps() : m_file_buffer(nullptr), m_file_size(0) {};
protected:
	char* m_file_buffer = nullptr;
	size_t m_file_size = 0;
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