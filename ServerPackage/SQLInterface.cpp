#include "SQLInterface.h"

#include <sql.h>
#include <iostream>
#include <sqlext.h>

#include "db_credentials.h"

// https://www.ibm.com/docs/en/db2-for-zos/13?topic=functions-sqlallochandle-allocate-handle

SQLInterface* SQLInterface::m_instance = nullptr;
char* SQLInterface::m_inConnStr = nullptr;

void getbobby(SQLHDBC _connection);

SQLInterface::SQLInterface()
{
	ConnectToDB();
	CreateConnectionString();
}


SQLInterface::~SQLInterface()
{
	delete m_environment_handle;
	delete m_database_connection_handle;
}

void SQLInterface::ConnectToDB()
{
	// The naming convention is:
	// SQL + h(andle) + handleType
	SQLHENV hEnv = nullptr;	// SQL Handle: Environment
	SQLHDBC hDbc = nullptr;	// SQL Handle: Database Connection

	SQLRETURN henvironment_state;
	SQLRETURN hconnection_state;
	SQLRETURN connection_state;
	SQLRETURN driver_state;

	henvironment_state = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
	SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);

	hconnection_state = SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);

	char debug[1024] = { 0 };
	SQLSMALLINT* new_len = nullptr;
	
	driver_state = SQLDriverConnectA(hDbc, nullptr, (SQLCHAR*)dsn, SQL_NTS, 
		(SQLCHAR*)debug, 1024, new_len, //Gives information
		SQL_DRIVER_COMPLETE);

	
	connection_state = SQLConnectA(hDbc,
		(SQLCHAR*)db_name, sizeof(db_name),
		(SQLCHAR*)auth_username, sizeof(auth_username),
		(SQLCHAR*)auth_password, sizeof(auth_password)
	);

	std::cerr << "SQL Server diagnostics:\n";
	InterpretState(henvironment_state, "environment handle");
	InterpretState(hconnection_state, "connection handle");
	InterpretState(driver_state, "Driver state");
	InterpretState(connection_state, "connection");

	getbobby(hDbc);

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
	
	std::cerr << (indented ? '\t' : '\0');
	std::cerr << name << ": " << error_message << " -- Code: " << code << '\n';
}

//TODO: This might be unnecessary
void SQLInterface::CreateConnectionString()
{
	if (m_inConnStr)
		return;

	m_inConnStr = new char[sizeof(dsn)];

}

void getbobby(SQLHDBC _connection)
{
	SQLHSTMT statement = nullptr;
	SQLAllocHandle(SQL_HANDLE_STMT, _connection, &statement);
	if (!statement)
		std::cerr << "Couldn't get bobby :(" << '\n';

	char query[] = "SELECT 1";
	SQLRETURN result = SQLExecDirect(statement,  (SQLWCHAR*)query, sizeof(query));
}