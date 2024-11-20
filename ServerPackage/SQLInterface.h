#pragma once

#include <Windows.h>
#include <sqltypes.h>
#include <iostream>

class SQLInterface
{
public:
	static SQLInterface* Instance() { if (!m_instance) m_instance = new SQLInterface(); return m_instance; };
	static void Destroy() { if (m_instance) delete m_instance; std::cerr << "destroyed SQL interface instance."; };

private:
	void ConnectToDB();
	void InterpretState(const SQLRETURN code, const char* name, const bool indented = true);


	SQLInterface();
	~SQLInterface();
	
	static SQLInterface* m_instance;
	SQLHENV m_hEnv = nullptr;	// Environment handle
	SQLHDBC m_hDbc = nullptr;	// Database Connection handle
};

