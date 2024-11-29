#pragma once

class FileOps
{
public:
protected:
	FileOps();
};

class EnvironmentFile : private FileOps
{
public:
	static EnvironmentFile* Instance() { return m_instance; };
	static EnvironmentFile* Destroy() { delete m_instance; };
	
	const char* FetchEnvironmentVariable(const char* variable);

private:
	static EnvironmentFile* m_instance;
	char* m_environment;

	void LoadEnvironment();
	EnvironmentFile();
};