#include "SQLInterface.h"

#include <sql.h>
#include <iostream>
#include <odbcss.h>		
#include <odbcinst.h>
#include <sqlext.h>

#include "db_credentials.h"

// https://www.ibm.com/docs/en/db2-for-zos/13?topic=functions-sqlallochandle-allocate-handle

SQLInterface* SQLInterface::m_instance = nullptr;
char* SQLInterface::m_inConnStr = nullptr;

void getbobby(SQLHDBC _connection);

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
	// SQL + h(andle) + handleType
	SQLHENV hEnv = nullptr;	// SQL Handle: Environment
	SQLHDBC hDbc = nullptr;	// SQL Handle: Database Connection

	SQLRETURN henvironment_state;
	SQLRETURN hconnection_state;
	SQLRETURN connection_state;
	SQLRETURN driver_state;

	//Set up environment
	henvironment_state = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
	SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);

	// This code is for when the DSN is unknown for the device
	// Configure your DSN in "ODBC Data Source Administrator (64-bit)"
	//		and put your defined name in the dsn field of `db_credentials.h`

		////Fetch data source
		//SQLRETURN data_source_state;
		//char DSN_buffer[255] = { 0 };
		//short int buffer_size;
		//data_source_state = SQLDataSourcesA(hEnv, SQL_FETCH_NEXT, (SQLCHAR*)DSN_buffer, sizeof(DSN_buffer) - 1, (SQLSMALLINT*)&buffer_size,
		//	nullptr, 0, nullptr); //This is for the DB description, which we don't really need.
		//if (data_source_state == SQL_SUCCESS)
		//	std::cout << "\t\t" << DSN_buffer << '\n';
		//InterpretState(data_source_state, "data source");
	
	//Get database connection handle
	hconnection_state = SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Connect to the database

	char debug[1024] = { 0 };
	SQLSMALLINT* new_len = nullptr;
	driver_state = SQLDriverConnectA(hDbc, nullptr, (SQLCHAR*)inConnStr, SQL_NTS,
		(SQLCHAR*)debug, 1024, new_len, //Gives information
		SQL_DRIVER_COMPLETE);

	//Other connect to the database
	connection_state = SQLConnectA(hDbc,
		(SQLCHAR*)db_name,			sizeof(db_name),
		(SQLCHAR*)auth_username,	sizeof(auth_username),
		(SQLCHAR*)auth_password,	sizeof(auth_password)
	);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	std::cerr << "SQL Server diagnostics:\n";
	InterpretState(henvironment_state, "environment handle");
	InterpretState(hconnection_state, "connection handle");
	InterpretState(driver_state, "driver state");
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


void getbobby(SQLHDBC _connection)
{
	SQLHSTMT statement = nullptr;
	SQLAllocHandle(SQL_HANDLE_STMT, _connection, &statement);
	if (!statement)
		std::cerr << "Couldn't get bobby :(" << '\n';

	char query[] = "SELECT 1";
	SQLRETURN result = SQLExecDirectA(statement,  (SQLCHAR*)query, sizeof(query));
}