#pragma once

class FileOps
{
public:
	static FileOps* Instance() { return m_instance; };
	static FileOps* Destroy() { delete m_instance; };

	const char* FetchEnvironmentVariable(const char* variable);

private:
	char* m_environment;
	void LoadEnvironment();

	static FileOps* m_instance;
	FileOps();


};

