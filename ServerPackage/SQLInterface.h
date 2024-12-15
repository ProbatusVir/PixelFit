#pragma once

#include <Windows.h>
#include <sqltypes.h>
#include <iostream>
#include <vector>

/// <summary>
/// TODO: Change the const char* in InsertNewUser and Fetch User statement to target 
/// the pixelFit mdf file, I could not migrate that file into mine, therefore it is setup to User.
/// All that needs changed is the part that is [User] these brackets are essental to finding the table
/// </summary>

class SQLInterface
{
public:
	static SQLInterface* Instance() { if (!m_instance) m_instance = new SQLInterface(); return m_instance; };
	static void Destroy() { if (m_instance) delete m_instance; std::cerr << "destroyed SQL interface instance."; };

	void FetchUser(const char* query);
	bool InsertNewUser(const char* name, const char* username, const char* email, const char* password);
	bool LoginRequest(const char* username, const char* password);
	void GetEveryExistingUsername(const char* query);

private:
	void ConnectToDB();
	void InterpretState(const SQLRETURN code, const char* name, const bool indented = true);
	void LoadCredentials();
	void ErrorLogFromSQL(SQLHSTMT& statement, SQLRETURN error);

	SQLHSTMT SetupAlloc(); 	// I (Ryan) got tired of writng the same 2 lines and large macros so I buried it into a function call
	std::vector<std::string> ReturnEval(SQLRETURN result, SQLHSTMT& statement);
	void HandleBindOfChars(SQLHSTMT& statement, int param, int columnWidth, const char* data);
	void HandleBindOfIntegers(SQLHSTMT& statement, int param, int columnWidth, const int data);
	void ResetHandle(SQLHSTMT& statement);	// Created to make resetting the handle more convientent compared to the other way

	SQLInterface();
	~SQLInterface();

	static SQLInterface* m_instance;

	const char* connStr = nullptr;
	SQLHENV m_hEnv = nullptr;	// Environment handle
	SQLHDBC m_hDbc = nullptr;	// Database Connection handle
};

