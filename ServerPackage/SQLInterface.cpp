#include "SQLInterface.h"

#include <sql.h>
#include <iostream>

#include "db_credentials.h"

// https://www.ibm.com/docs/en/db2-for-zos/13?topic=functions-sqlallochandle-allocate-handle

SQLInterface* SQLInterface::m_instance = nullptr;

SQLInterface::SQLInterface()
{
	ConnectToDB();
}

SQLInterface::~SQLInterface()
{
	delete m_environment_handle;
	delete m_database_connection_handle;
}

void SQLInterface::ConnectToDB()
{
	// The naming convention is:
	// SQL + Handle + handletType
	SQLHENV hEnv = nullptr;	// SQL Handle: Environment
	SQLHDBC hDbc = nullptr;	// SQL Handle: Database Connection

	SQLRETURN henvironment_state;
	SQLRETURN hconnection_state;
	SQLRETURN connection_state;


	henvironment_state = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
	hconnection_state = SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);

	connection_state = SQLConnect(hDbc,
		(SQLCHAR*)db_name, sizeof(db_name),
		(SQLCHAR*)auth_username, sizeof(auth_username),
		(SQLCHAR*)auth_password, sizeof(auth_password)
	);

	std::cerr << "SQL Server diagnostics:\n";
	InterpretState(henvironment_state, "environment handle");
	InterpretState(henvironment_state, "connection handle");
	InterpretState(henvironment_state, "connection");

}

void SQLInterface::InterpretState(const SQLRETURN code, const char* name, const bool indented)
{
	const char* error_message;
	switch (code)
	{
	case (SQL_INVALID_HANDLE): error_message = "Invalid handle";		break;
	case (SQL_SUCCESS): error_message = "success";						break;
	case(SQL_SUCCESS_WITH_INFO): error_message = "success with info";	break;
	default: error_message = "error initializing";						break;
	}
	
	std::cerr << (indented ? '\t' : '\0') << name << ": " << error_message << " -- Code: " << code << '\n';
}

