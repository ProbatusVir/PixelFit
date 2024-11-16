#include "SQLInterface.h"

#include <sql.h>
#include <iostream>

#include "db_credentials.h"

// https://www.ibm.com/docs/en/db2-for-zos/13?topic=functions-sqlallochandle-allocate-handle

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

	InterpretState(henvironment_state, "environment handle");
	InterpretState(henvironment_state, "connection handle");
	InterpretState(henvironment_state, "connection");

}

void SQLInterface::InterpretState(const SQLRETURN code, const char* name)
{
	switch (code)
	{
	case (SQL_INVALID_HANDLE): std::cerr << "Invalid handle for " << name << '\n';		break;
	case (SQL_SUCCESS): std::cerr << name << " succeeded!" << '\n';						break;
	case(SQL_SUCCESS_WITH_INFO): std::cerr << name << " succeeded with info!" << '\n';	break;
	default: std::cerr << "Something went wrong initializing " << name << '\n';			break;
	}
}

