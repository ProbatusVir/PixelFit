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

	void FetchUser(const char* query) const;
	bool InsertNewUser(const char* name, const char* username, const char* email, const char* password) const;
	bool LoginRequest(const char* username, const char* password) const;
	std::vector<std::string> GetEveryExistingUsername() const;
	std::vector<std::string> GetEveryUserContaining(const char* substr) const;
	void BlockUser(const char* username1, const char* username2) const;
	void ToggleFriendUser(const char* username1, const char* username2) const;

private:
	int GetUserIDByUsername(const char* username) const;
	void ConnectToDB();
	void InterpretState(const SQLRETURN code, const char* name, const bool indented = true) const;
	void LoadCredentials();
	void ErrorLogFromSQL(const SQLHSTMT statement, const SQLRETURN error) const;

	SQLHSTMT SetupAlloc() const; 	// I (Ryan) got tired of writng the same 2 lines and large macros so I buried it into a function call
	std::vector<std::string> ReturnEval(const SQLRETURN result, const SQLHSTMT statement) const;
	void HandleBindOfChars(const SQLHSTMT statement, const int param, const int columnWidth, const char* data) const;
	void HandleBindOfIntegers(const SQLHSTMT statement, const int param, const int columnWidth, const int data) const;
	void ResetHandle(SQLHSTMT& statement);	// Created to make resetting the handle more convientent compared to the other way

	void HandleFail(const char* failmessage, const SQLHSTMT statement, const SQLRETURN result, const char* additional = "") const;

	SQLInterface();
	~SQLInterface();

	static SQLInterface* m_instance;

	const char* connStr = nullptr;
	SQLHENV m_hEnv = nullptr;	// Environment handle
	SQLHDBC m_hDbc = nullptr;	// Database Connection handle
};

