#include "DatabaseManager.hpp"
#include "PasswordSecurity.hpp"
#include <iostream>

int main()
{
	DatabaseManager database("test_database.db");

	database.createTableIfNotExists
	(
		"Users",
        "user_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "user_name TEXT UNIQUE NOT NULL CHECK(length(user_name) <= 32)"
	);

	std::cout << "Successful creation of 'Users' table." << std::endl;

	database.createTableIfNotExists
	(
		"Clients",
		"client_id INTEGER PRIMARY KEY AUTOINCREMENT,"
		"client_name TEXT UNIQUE NOT NULL CHECK(length(user_name) <= 32)"
		"errant text"
	);
	
	std::cout << "Successful creation of 'Clients' table." << std::endl;

}