#pragma once

#include <Windows.h>
#include <sqltypes.h>

class SQLInterface
{
public:
	static SQLInterface* Instance() { if (!m_instance) m_instance = new SQLInterface(); return m_instance; };
	static void Destroy() { if (m_instance) delete m_instance; };

private:
	void ConnectToDB();
	void InterpretState(const SQLRETURN code, const char* name, const bool indented = true);

	SQLInterface();
	~SQLInterface();

	static SQLInterface* m_instance;
	SQLHENV m_environment_handle = nullptr;
	SQLHDBC m_database_connection_handle = nullptr;
};

