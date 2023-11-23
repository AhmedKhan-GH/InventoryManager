#include "DatabaseManager.hpp"

#include <iostream>

int main()
{

	DatabaseManager database("test_database.db");
	database.createTableIfNotExists
	(
		"Users",
        "user_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "user_name TEXT UNIQUE NOT NULL CHECK(length(user_name) <= 32),"
        "user_salt TEXT UNIQUE NOT NULL CHECK(length(user_salt) <= 32),"
        "user_passhash TEXT UNIQUE NOT NULL CHECK(length(user_passhash) <= 64),"
        "user_timestamp DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,"
        "user_visibility BOOLEAN NOT NULL DEFAULT 1,"
        "user_legalname TEXT CHECK(length(user_legalname) <= 64),"
        "user_phonenumber TEXT CHECK(length(user_phonenumber) <= 16),"
        "user_emailaddress TEXT CHECK(length(user_emailaddress) <= 256),"
        "user_description TEXT,"
        "user_permission TEXT NOT NULL DEFAULT 'BASE' CHECK(user_permission IN ('LOCK', 'BASE', 'SUPER', 'ADMIN'))"
	);


}