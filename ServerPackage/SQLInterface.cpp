#include "SQLInterface.h"
#include "Constants.h"
#include "TokenHelper.h"
#include "FileOps.h"
#include "Header.h" //This is pretty much just for the concatenation... maybe I should put this somewhere else...

#include <sql.h>
#include <odbcss.h>		
#include <odbcinst.h>
#include <sqlext.h>
#include <fstream>
#include <filesystem>

// https://www.ibm.com/docs/en/db2-for-zos/13?topic=functions-sqlallochandle-allocate-handle

SQLInterface* SQLInterface::m_instance = nullptr;

SQLInterface::SQLInterface()
{
	LoadCredentials();
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

	// This looks for the .env file in the directory and will load your credentials to then get access to the db
	SQLRETURN henvironment_state;
	SQLRETURN hconnection_state;
	//SQLRETURN connection_state;
	SQLRETURN driver_state;

	//Set up environment
	henvironment_state = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_hEnv);
	SQLSetEnvAttr(m_hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);

	// This code is for when the DSN is unknown for the device
	// Configure your DSN in "ODBC Data Source Administrator (64-bit)"
	//		and put your defined name in the dsn field of `db_credentials.h`
	
	//Get database connection handle
	hconnection_state = SQLAllocHandle(SQL_HANDLE_DBC, m_hEnv, &m_hDbc);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	//Connect to the database
	char debug[1024] = { 0 };
	SQLSMALLINT new_len;
	driver_state = SQLDriverConnectA(m_hDbc, nullptr, (SQLCHAR*)connStr, SQL_NTS,
		(SQLCHAR*)debug, 1024, &new_len, //Gives information
		SQL_DRIVER_COMPLETE);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	std::cerr << "SQL Server diagnostics:\n";
	InterpretState(henvironment_state, "environment handle");
	InterpretState(hconnection_state, "connection handle");
	InterpretState(driver_state, "driver state");
	

}

void SQLInterface::InterpretState(const SQLRETURN code, const char* name, const bool indented)
{
	const char* error_message; //Not SQL related
	char successWithInfo[1024] = { 0 };
	SQLCHAR sqlState[6];
	SQLINTEGER nativeError;
	SQLCHAR message[1024];
	SQLSMALLINT textLength;


	if (code == SQL_SUCCESS_WITH_INFO || code == SQL_ERROR) {
		SQLGetDiagRecA(SQL_HANDLE_DBC, m_hDbc, 1, sqlState, &nativeError, message, sizeof(message), &textLength);
		std::cout << "Warning: " << (char*)message << " (SQL State: " << (char*)sqlState << ")\n";
	}

	std::cout << successWithInfo << '\n';

	switch (code)
	{
	case (SQL_INVALID_HANDLE):  error_message = "Invalid handle";		break;
	case (SQL_SUCCESS): error_message = "success";						break;
	case(SQL_SUCCESS_WITH_INFO): error_message = "success with info";	break;
	default: error_message = "error initializing";						break;
	}


	
	std::cerr << (indented ? '\t' : '\0');
	std::cerr << name << ": " << error_message << " -- Code: " << code << '\n';
}
// Loads the .env file so the credentials are not on GitHub
void SQLInterface::LoadCredentials()
{
	EnvironmentFile* loader = EnvironmentFile::Instance();

	constexpr const char field1[] = "DSN=";
	constexpr const char field2[] = ";Trusted_Connection=Yes;WSID=";
	constexpr const char end[] = ";";
	

	const char* dsn = loader->FetchEnvironmentVariable("dsn");
	const char* db_name = loader->FetchEnvironmentVariable("db_name");
	
	const char* components[] = {	field1,		dsn,			field2,				db_name, end };
	const size_t sizes[] = { sizeof(field1) - 1, strlen(dsn),	sizeof(field2) - 1, strlen(db_name), sizeof(end) };
	connStr = CONCATENATEA(components, sizes);
}
bool SQLInterface::LoginRequest(const char* username, const char* password)
{
	SQLHSTMT statement = SetupAlloc();
	bool isValid = false;
	// TODO: UPDATE THE [User] to target your dbo.[insert db table name here]
	constexpr const char checkForUsername[] = "SELECT nvcUserName, nvcPasswordHash FROM dbo.[tblUser] WHERE nvcUserName = ? AND nvcPasswordHash = ?";

	SQLRETURN result = SQLPrepareA(statement, (SQLCHAR*)checkForUsername, SQL_NTS);

	HandleBindOfChars(statement, 1, USERNAME_SIZE, username);
	HandleBindOfChars(statement, 2, HASH_SIZE, password);

	result = SQLExecDirectA(statement, (SQLCHAR*)checkForUsername, SQL_NTS);
	
	result = SQLFetch(statement);
	if (result == SQL_SUCCESS || result == SQL_SUCCESS_WITH_INFO) {
		SQLFreeHandle(SQL_HANDLE_STMT, statement);
		std::cerr << "Successfully authenticated user: " << username << '\n';
		return true;
	}
	else {
		ErrorLogFromSQL(statement, result);
		SQLFreeHandle(SQL_HANDLE_STMT, statement);
		return false;
	}


	

}
std::vector<std::string> SQLInterface::GetEveryExistingUsername()
{
	constexpr const char query[] = "SELECT nvcUserName FROM [dbo].[tblUser]";
	SQLHSTMT statement = SetupAlloc();
	SQLRETURN result = SQLExecDirectA(statement, (SQLCHAR*)query, sizeof(query));
	return ReturnEval(result, statement);
}
/// <summary>
/// Attempts to make a new user, if the username already exists this will fail and kick back a false 
/// This forces users to have different usernames
/// </summary>
/// <param name="name"></param>
/// <param name="username"></param>
/// <param name="password"></param>
/// <param name="id"></param>
/// <returns></returns>
bool SQLInterface::InsertNewUser(const char* name, const char* username, const char* email, const char* password)
{
	bool userNotRegistered = true;
	bool isValid = false;
	std::vector<std::string> usernames = GetEveryExistingUsername();
	
	for (std::string& registered : usernames) {
		
		//All strings are space padded if they are smaller than the allotted size. so they must be trimmed.
		const size_t trail_start = registered.find(' ');
		if (trail_start != std::string::npos)
			registered.erase(trail_start);

		int compareResult = strcmp(username, registered.c_str());
		// We want to compare the usernames to enforce one username is registered to one person.
		// We do not want multiple usernames to multiple users so instead of wanting strcmp to == 0 we want -1 or 1
		if (compareResult == 0) {

			// Indicates the user is already existing and we will not bind this name to the new person
			userNotRegistered = false;
			break;
		}
	}

	// This section handles if the user does not exist and puts them into the db
	if (userNotRegistered) {
		constexpr const char insertSqlCmd[] = "INSERT INTO [dbo].[tblUser] (nvcName, nvcEmailAddress, nvcUserName, nvcPasswordHash) VALUES (?,?,?,?)";
		SQLHSTMT statement = SetupAlloc();
		SQLRETURN result = SQLPrepareA(statement, (SQLCHAR*)insertSqlCmd, sizeof(insertSqlCmd));
		if (result != SQL_SUCCESS) {
			std::cerr << "Error with adding to db\n";
			ErrorLogFromSQL(statement, result);
			SQLFreeHandle(SQL_HANDLE_STMT, statement);
			return false;
		}


		// SQLBindParameter binds to the insertSQLCmd string which fills in each of the (?) in the paranthesis.
		// Each parameter must be bound and I did not find a a better way to handle this.

		//iUserID	nvcName	nvcEmailAddress	nvcUserName nvcPasswordHash

		HandleBindOfChars(statement, 1, NAME_SIZE, name);
		HandleBindOfChars(statement, 2, EMAIL_SIZE, email);
		HandleBindOfChars(statement, 3, USERNAME_SIZE, username);
		HandleBindOfChars(statement, 4, HASH_SIZE, password);

		result = SQLExecute(statement);
		if (result == SQL_SUCCESS || result == SQL_SUCCESS_WITH_INFO) {
			isValid = true;
		}
		else {
			ErrorLogFromSQL(statement, result);
		}

		SQLFreeHandle(SQL_HANDLE_STMT, statement);
	}
	else
		std::cerr << "User not created due to existing user with the same username." << '\n';

	// Always free the handle when we are done with the function
	return isValid;
}
/// <summary>
/// This method is because typing the same three lines is redundant
/// </summary>
/// <returns></returns>
SQLHSTMT SQLInterface::SetupAlloc()
{
	SQLHSTMT statement = nullptr;
	SQLAllocHandle(SQL_HANDLE_STMT, m_hDbc, &statement);
	return statement;
}

// Since nearly every SQL call that goes to the database will need evaluated
// This is made a little specific to users at this point though
std::vector<std::string> SQLInterface::ReturnEval(SQLRETURN result, SQLHSTMT& statement)
{
	std::vector<std::string> data;
	if (result == SQL_SUCCESS || result == SQL_SUCCESS_WITH_INFO) {
		SQLCHAR columnData[PASSWORD_SIZE];
		SQLLEN indicator;
		SQLBindCol(statement,
			1,
			SQL_C_CHAR,
			columnData,
			sizeof(columnData),
			&indicator);
		while (SQLFetch(statement) == SQL_SUCCESS) {
			if (indicator == SQL_NULL_DATA) {

			}
			else {
				data.push_back((char*)columnData);
			}
		}
	}
	//Pushing back "Error" is even worse than having a 0 sized vector...
	return data;
}
// The entire purpose of this is to again reduce redundancies and bury stuff that isnt going to change
void SQLInterface::HandleBindOfChars(SQLHSTMT& statement, int param, int columnWidth, const char* data)
{
	SQLBindParameter(statement, param, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, columnWidth, 0, (SQLPOINTER)data, sizeof(data), nullptr);
}


// This is to abstract some of the annoyance of needing 10 parameters everytime down to about four parameters
void SQLInterface::HandleBindOfIntegers(SQLHSTMT& statement, int param, int columnWidth, const int data)
{
	SQLLEN dataLength = sizeof(SQLINTEGER);
	SQLBindParameter(statement, param, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, (SQLPOINTER)&data, 0, &dataLength);
}


void SQLInterface::ErrorLogFromSQL(const SQLHSTMT& statement, const SQLRETURN error)
{
	const char* error_message = nullptr;

	switch (error)
	{
	case(SQL_NEED_DATA):
		error_message = "Insufficient data/fields provided!";
		break;
	case(SQL_STILL_EXECUTING):
		error_message = "SQL query still running!";
		break;
	case (SQL_NO_DATA):
		error_message = "No data received back from query!";
		break;
	case (SQL_INVALID_HANDLE):
		error_message = "Invalid handle!";
		break;
	case (SQL_ERROR) :
		InterpretState(error, "");
		return;
	default:
		error_message = "Undiagnosed.";
		break;
	}

	std::cout << "SQL ERROR " << error << ": " << error_message << '\n';
}

// Because Microsofts version of this does not work so I made one that does
void SQLInterface::ResetHandle(SQLHSTMT& statement)
{
	SQLFreeHandle(SQL_HANDLE_STMT, statement);
	statement = SetupAlloc();
}


// As it implies, it fetches users. NOTE: This has a statement that needs adjusted because I could not integrate the mdf file
void SQLInterface::FetchUser(const char* query)
{

	// This function needs slight modification to be useful for our needs
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
		ErrorLogFromSQL(statement, result); //Historically, this has always failed.

	}

	SQLFreeHandle(SQL_HANDLE_STMT, statement);

}
