#pragma once

#include <Windows.h>
#include <sqltypes.h>

class SQLInterface
{
	void ConnectToDB();
	void InterpretState(const SQLRETURN code, const char* name);

	SQLHENV m_environment_handle = nullptr;
	SQLHDBC m_database_connection_handle = nullptr;
};

