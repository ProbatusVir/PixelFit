#include "SQLInterface.h"

#include <sql.h>
#include <iostream>
#include <odbcss.h>		
#include <odbcinst.h>
#include <sqlext.h>
#include <fstream>
#include <string>

//#define db_name "MSI"
//#define auth_username "MSI/Rsube"
//#define auth_password ""
//#define dsn "DB"


// https://www.ibm.com/docs/en/db2-for-zos/13?topic=functions-sqlallochandle-allocate-handle

SQLInterface* SQLInterface::m_instance = nullptr;

void getbobby(SQLHDBC _connection);

SQLInterface::SQLInterface()
{
	ConnectToDB();
}


SQLInterface::~SQLInterface()
{
	SQLFreeHandle(SQL_HANDLE_DBC, m_hDbc);
	SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv);

	if (m_hDbc)
		delete m_hDbc; 
	if (m_hEnv)
		delete m_hEnv;
	if (connStr != nullptr) delete[] connStr;
}

void SQLInterface::ConnectToDB()
{
	// The naming convention is:
	// SQL + h(andle) + handleType
	LoadCredentials(".env");
	SQLRETURN henvironment_state;
	SQLRETURN hconnection_state;
	SQLRETURN connection_state;
	SQLRETURN driver_state;

	//Set up environment
	henvironment_state = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_hEnv);
	SQLSetEnvAttr(m_hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);

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
	hconnection_state = SQLAllocHandle(SQL_HANDLE_DBC, m_hEnv, &m_hDbc);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Connect to the database

	char debug[1024] = { 0 };
	SQLSMALLINT new_len;
	driver_state = SQLDriverConnectA(m_hDbc, nullptr, (SQLCHAR*)connStr, SQL_NTS,
		(SQLCHAR*)debug, 1024, &new_len, //Gives information
		SQL_DRIVER_COMPLETE);

	//Other connect to the database
	// Only one of these methods should be used
	// The driver connect is better and easier to use as it uses trusted setting
	// The method below can work if you have login credentials, the one below is more so if you have cloud database
	//connection_state = SQLConnectA(m_hDbc,);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	std::cerr << "SQL Server diagnostics:\n";
	InterpretState(henvironment_state, "environment handle");
	InterpretState(hconnection_state, "connection handle");
	InterpretState(driver_state, "driver state");
	//InterpretState(connection_state, "connection");


//	getbobby(m_hDbc);

}

void SQLInterface::InterpretState(const SQLRETURN code, const char* name, const bool indented)
{
	const char* error_message;
	char succcssWithInfo[1024] = { 0 };
	SQLWCHAR sqlState[6];
	SQLINTEGER nativeError;
	SQLCHAR message[1024];
	SQLSMALLINT textLength;


	if (code == SQL_SUCCESS_WITH_INFO) {
		SQLGetDiagRec(SQL_HANDLE_DBC, m_hDbc, 1, (SQLWCHAR*)sqlState, &nativeError, (SQLWCHAR*)message, sizeof(message), &textLength);
		std::cout << "Warning: " << message << " (SQL State: " << (char*)sqlState << ")\n";
	}
	for (int i = 0; i < SQL_MAX_MESSAGE_LENGTH; i++) {
		if (message[i] = '\0') {
			succcssWithInfo[i] = ' ';
		}
		else succcssWithInfo[i] = message[i];
	}

	std::cout << succcssWithInfo << '\n';

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
// Loads the .env file so the credentials are not on github
// The order of data is as shown inside the function
void SQLInterface::LoadCredentials(const char* path)
{
	std::ifstream inputFile(path);
	std::string connectStr = "";
	if (inputFile.is_open()) {

		std::string dsn = "";
		std::string db_name = "";
		std::string buffer = "";
		int counter = 0;
		while (std::getline(inputFile, buffer)) {
			if (counter == 0) {
				dsn = buffer;
			}
			else db_name = buffer;
			counter++;
		}

	connectStr = "DSN=" + dsn + ";Trusted_Connection=Yes;WSID=" + db_name + ";";
	connStr = new char[connectStr.size() + 1];
	strcpy_s(connStr, connectStr.size() + 1, connectStr.c_str());
	};



	
}

void SQLInterface::FetchUser(const char* query)
{
	SQLHSTMT statement = nullptr;
	SQLAllocHandle(SQL_HANDLE_STMT, m_hDbc, &statement);

	SQLRETURN result = SQLExecDirectA(statement, (SQLCHAR*)query, SQL_NTS);

	if (result == SQL_SUCCESS || result == SQL_SUCCESS_WITH_INFO) {
		SQLCHAR columnData[256];
		SQLLEN indicator;

		SQLBindCol(statement,
			1,                      // Bind the first column
			SQL_C_CHAR,             // Target type: character string
			columnData,             // Pointer to buffer for data
			sizeof(columnData),     // Buffer size in bytes
			&indicator);            // Length/NULL indicator

		while (SQLFetch(statement) == SQL_SUCCESS) {
			if (indicator == SQL_NULL_DATA) {
				std::cout << "Column Data: NULL\n";
			}
			else {
				std::cout << "Column Data: " << columnData << '\n';
			}
		}
		std::cout << "End of data or no data exists \n";
	}
	else {
		SQLWCHAR sqlState[100]; // Ensure size is 6 (5 chars + null terminator)
		SQLINTEGER nativeError;
		SQLWCHAR message[SQL_MAX_MESSAGE_LENGTH];
		SQLSMALLINT textLength;

		SQLGetDiagRec(SQL_HANDLE_STMT, statement, 1, sqlState, &nativeError,  message, sizeof(message), &textLength);
		std::wcout << L"SQL State: " << sqlState << L", Native Error: " << nativeError << L", Message: " << message << std::endl;

	}

	SQLFreeHandle(SQL_HANDLE_STMT, statement);

}


void getbobby(SQLHDBC _connection)
{
	SQLHSTMT statement = nullptr;
	SQLAllocHandle(SQL_HANDLE_STMT, _connection, &statement);
	if (!statement)
		std::cerr << "Couldn't get bobby :(" << '\n';

	char query[] = "SELECT 1";
	SQLRETURN result = SQLExecDirectA(statement,  (SQLCHAR*)query, sizeof(query));

	//if (result == SQL_SUCCESS) {
	//	std::cout << query << '\n';
	//}
	//else {
	//	std::cout << "See more info \n";
	//}
}