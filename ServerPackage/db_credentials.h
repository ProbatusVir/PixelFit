#pragma once
#include <fstream>
// If you don't know how to access your database credentials:
// https://www.dropbox.com/scl/fi/wlozzlt77ib21d1wdj22n/login_credentials.png?rlkey=wkqn7m9pv9rc1ob8fk8qw0lfx&st=w7pg8y69&dl=0
// Make sure your server is properly started before you attempt to log in, or it simply won't work.

#define db_name ""
#define auth_username ""
#define auth_password ""
#define dsn ""

// Don't touch.
#define field1 "DSN="
#define field2 ";UID="
#define field3 ";Trusted_Connection=Yes"
#define field4 ";WSID="

//DSN=LOCAL_DSN;UID=USER;Trusted_Connection=Yes;WSID=SERVERHOST;


//const char* inConnStr = LoadCredentials(".env");
//constexpr const char* inConnStr = field1 dsn field2 auth_username field3 field4 db_name;