#include "DatabaseManager.hpp"

#include <iostream>

int main()
{

	DatabaseManager database("test_database.db");
	database.createTableIfNotExists
	(
		"Users",
		"user_id INT PRIMARY KEY AUTO_INCREMENT,"
		"user_name VARCHAR(32) UNIQUE NOT NULL,"
		"CONSTRAINT username_allowed_chars CHECK (username ~ '^[a-zA-Z0-9_]+$'),"
		"user_salt VARCHAR(32) UNIQUE NOT NULL,"
		"user_passhash VARCHAR(64) UNIQUE NOT NULL,"
		"user_timestamp DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,"
		"user_visibility BOOLEAN NOT NULL DEFAULT 1,"
		"user_legalname VARCHAR(64),"
		"user_phonenumber VARCHAR(16),"
		"CONSTRAINT phonenumber_allowed_chars CHECK(user_phonenumber ~'^[0-9]+$'),"
		"user_emailaddress VARCHAR(256),"
		"user_desription TEXT,"
		"user_permission ENUM('LOCK', 'BASE', 'SUPER', 'ADMIN') NOT NULL DEFAULT 'BASE'"
	);


}