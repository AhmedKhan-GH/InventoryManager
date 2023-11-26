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
		"user_age INT UNIQUE NOT NULL,
		"user_height DOUBLE UNIQUE NOT NULL"
	);

	std::cout << "Successful creation of 'Users' table." << std::endl;

	const std::string insertUserSQL = "INSERT INTO Users (user_name, user_age, user_height) VALUES (?, ?, ?)";
	database.prepareStatement(insertUserSQL);

	database.bindString(1, "John");
	database.bindInt(2, 21);
	database.bindDouble(3, 175.5);

	database.executePrepared();

	std::cout << "Successful insertion of user into table" << std::endl;

}