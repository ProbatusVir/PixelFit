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
#include <ctype.h>

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

static_assert(sizeof(long int) == sizeof(int));

int SQLInterface::GetUserIDByUsername(const char* username) const
{
	constexpr const char query_start[] = "SELECT iUserID FROM dbo.[tblUser] WHERE nvcUserName = '";
	constexpr const char query_end[] = "'";
	const char* components[] = { query_start, username, query_end };
	const size_t sizes[] = { sizeof(query_start) - 1, strlen(username), sizeof(query_end)};
	
	const char* query = CONCATENATEA(components, sizes);

	SQLHSTMT statement = SetupAlloc();
	SQLRETURN result = SQLExecDirectA(statement, (SQLCHAR*)query, sizes[0] - 1 + sizes[1] + sizes[2]);
	
	ErrorLogFromSQL(statement, result);

	int id = -1;
	SQLLEN len = -1;

	result = SQLBindCol(statement, 1, SQL_C_SLONG, &id, sizeof(id), &len);
	ErrorLogFromSQL(statement, result);
	result = SQLFetch(statement);
	ErrorLogFromSQL(statement, result);




	SQLFreeHandle(SQL_HANDLE_STMT, statement);
	delete[] query;

	return id;
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

	//DEBUG
	//int a = GetUserIDByUsername("NotKotlinusername");
	BlockUser("NotKotlinusername", "blahblah@gmail.com");

	}

void SQLInterface::InterpretState(const SQLRETURN code, const char* name, const bool indented) const
{
	const char* error_message; //Not SQL related
	char successWithInfo[1024] = { 0 };
	SQLCHAR sqlState[6];
	SQLINTEGER nativeError;
	SQLCHAR message[1024];
	SQLSMALLINT textLength;


	if (code == SQL_SUCCESS_WITH_INFO ) {
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
bool SQLInterface::LoginRequest(const char* username, const char* password) const
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
std::vector<std::string> SQLInterface::GetEveryExistingUsername() const
{
	constexpr const char query[] = "SELECT nvcUserName FROM [dbo].[tblUser]";
	SQLHSTMT statement = SetupAlloc();
	SQLRETURN result = SQLExecDirectA(statement, (SQLCHAR*)query, sizeof(query));
	return ReturnEval(result, statement);
}


std::vector<std::string> SQLInterface::GetEveryUserContaining(const char* substr) const
{
	std::vector<std::string> hits;
	//constexpr const char query[] = "SELECT nvcUserName FROM tblUser WHERE nvcUserName LIKE '%?%'";
	//constexpr const char query[] = "SELECT nvcUserName FROM tblUser WHERE nvcUserName LIKE '%kot%'";

	//This is super hacky and I hate it. But it seems that binding data inside string literals isn't easy...
	constexpr const char query_start[] = "SELECT nvcUserName FROM tblUser WHERE nvcUserName LIKE '%";
	constexpr const char query_end[] = "%'";
	const char* data[] = { query_start, substr, query_end };
	const size_t sizes[] = { sizeof(query_start) - 1, strlen(substr), sizeof(query_end) };
	const char* query = CONCATENATEA(data, sizes);


	SQLHSTMT statement = SetupAlloc();
	SQLRETURN result = SQLPrepareA(statement, (SQLCHAR*)query, SQL_NTS);


	if (result != SQL_SUCCESS) {
		std::cerr << "Failed to retrieve users containing: " << substr;
		ErrorLogFromSQL(statement, result);
		SQLFreeHandle(SQL_HANDLE_STMT, statement);
		return hits;
	}

	//HandleBindOfChars(statement, 1, strlen(substr), substr);
	//HandleBindOfChars(statement, 1, USERNAME_SIZE, substr);

	result = SQLExecute(statement);
	if (result == SQL_SUCCESS || result == SQL_SUCCESS_WITH_INFO)
		hits = ReturnEval(result, statement);
	else
		ErrorLogFromSQL(statement, result);

	delete[] query;
	SQLFreeHandle(SQL_HANDLE_STMT, statement);
	return hits;
}




//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
void SQLInterface::BlockUser(const char* blocker, const char* blocked) const
{
	if (strcmp(blocker, blocked) == 0)
		return;

	const int blockerID = GetUserIDByUsername(blocker);
	const int blockedID = GetUserIDByUsername(blocked);

	const int lesserID = (blockerID < blockedID) ? blockerID : blockedID;
	const int greaterID = (blockedID > blockerID) ? blockedID : blockerID;

	constexpr const char query[] = "INSERT INTO [dbo].[tblBlock] (iLesserID, iGreaterID) VALUES (?,?)";
	
	SQLHSTMT statement = SetupAlloc();
	SQLRETURN result = SQLPrepareA(statement, (SQLCHAR*)query, sizeof(query));
	ErrorLogFromSQL(statement, result);

	result = SQLBindParameter(statement, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, (SQLPOINTER)&lesserID, 0,  nullptr);
	ErrorLogFromSQL(statement, result);
	result = SQLBindParameter(statement, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, (SQLPOINTER)&greaterID, 0, nullptr);
	ErrorLogFromSQL(statement, result);

	result = SQLExecute(statement);

	if (result != SQL_SUCCESS)
		std::cerr << "Could not block user.\n";

	ErrorLogFromSQL(statement, result);
	SQLFreeHandle(SQL_HANDLE_STMT, statement);

	//delete[] query;
}

void SQLInterface::ToggleFriendUser(const char* username1, const char* username2) const
{
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
bool SQLInterface::InsertNewUser(const char* name, const char* username, const char* email, const char* password) const
{
	bool userNotRegistered = true;
	bool isValid = false;
	std::vector<std::string> usernames = GetEveryExistingUsername();
	
	for (std::string& registered : usernames) {
		
		//All strings are space padded if they are smaller than the allotted size. so they must be trimmed.
		trimstr(registered);

		if (strcmp(username, registered.c_str()) == 0) {

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
SQLHSTMT SQLInterface::SetupAlloc() const
{
	SQLHSTMT statement = nullptr;
	SQLAllocHandle(SQL_HANDLE_STMT, m_hDbc, &statement);
	return statement;
}

// Since nearly every SQL call that goes to the database will need evaluated
// This is made a little specific to users at this point though
std::vector<std::string> SQLInterface::ReturnEval(const SQLRETURN result, const SQLHSTMT statement) const
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
void SQLInterface::HandleBindOfChars(const SQLHSTMT statement, const int param, const int columnWidth, const char* data) const
{
	SQLRETURN result = SQLBindParameter(statement, param, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, columnWidth, 0, (SQLPOINTER)data, sizeof(data), nullptr);
	ErrorLogFromSQL(statement, result);
}


// This is to abstract some of the annoyance of needing 10 parameters everytime down to about four parameters
void SQLInterface::HandleBindOfIntegers(const SQLHSTMT statement, const int param, const int columnWidth, const int data) const
{
	//SQLRETURN result = SQLBindParameter(statement, param, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, (SQLPOINTER)&data, sizeof(data), nullptr);
	SQLRETURN result = SQLBindParameter(statement, param, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, (SQLPOINTER)&data, sizeof(data), nullptr);
	ErrorLogFromSQL(statement, result);
}


void SQLInterface::ErrorLogFromSQL(const SQLHSTMT statement, const SQLRETURN error) const
{
	const char* error_message = nullptr;

	if (error == SQL_SUCCESS || error == SQL_SUCCESS_WITH_INFO)
		return;

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
		SQLCHAR sql_state[6];
		SQLINTEGER native_error;
		SQLCHAR message[1024];
		SQLSMALLINT textLength;
		SQLGetDiagRecA(SQL_HANDLE_STMT, statement, 1, sql_state, &native_error, message, sizeof(message), &textLength);
		std::cerr << "ERROR: " << (char*)message << "\n\tNative Error: " << native_error << "\n\tSQL State: " << sql_state << '\n';
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

void SQLInterface::HandleFail(const char* failmessage, const SQLHSTMT statement, const SQLRETURN result, const char* additional) const
{
	std::cerr << failmessage << additional;
	ErrorLogFromSQL(statement, result);
	SQLFreeHandle(SQL_HANDLE_STMT, statement);
}


// As it implies, it fetches users. NOTE: This has a statement that needs adjusted because I could not integrate the mdf file
void SQLInterface::FetchUser(const char* query) const
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
