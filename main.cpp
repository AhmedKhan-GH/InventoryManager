#include "DatabaseManager.hpp"
#include "PasswordSecurity.hpp"
#include <iostream>
#include <map>

int main()
{
	DatabaseManager database("test_database.db");

	database.createTableIfNotExists
	(
		"Users",
		"user_id INTEGER PRIMARY KEY AUTOINCREMENT,"
		"user_name TEXT NOT NULL CHECK(length(user_name) <= 32),"
		"user_age INTEGER,"
		"user_height REAL"
	);

	std::map<int, DataType> user_parameter_indices =
	{
		{1, DataType::TEXT},  // user_name
		{2, DataType::INTEGER},  // user_age
		{3, DataType::REAL}  // user_height
	};

	std::string user_insert_sql = "INSERT INTO Users (user_name, user_age, user_height) VALUES (?, ?, ?)";
	
	std::cout << "Successful creation of 'Users' table." << std::endl;


	//demonstrates a successful user insertion
	database.prepareStatement(user_insert_sql, user_parameter_indices);
	database.bindString(1, "John");
	database.bindInt(2, 21);
	database.bindDouble(3, 175.5);
	if (database.executePrepared())
	{
		std::cout << "Successful insertion of user into table" << std::endl;
	}
	else
	{
		std::cout << "Unsuccessful insertion of user into table" << std::endl;
	}

	std::cout << "Successful insertion of user into table" << std::endl;

	//demonstrates a bad prepared statement query
	if(database.prepareStatement("bad sql", user_parameter_indices))
	{
		std::cout << "Successful statement preparation" << std::endl;
	}
	else
	{
		std::cout << "Unsuccessful statement preparation" << std::endl;
	}

	//demonstrates transgressing the parameter indices map
	database.prepareStatement("INSERT INTO Users (user_name, user_age, user_height) VALUES (?, ?, ?)", user_parameter_indices);
	database.bindString(3, "Henry");
	database.bindInt(2, 21);
	database.bindDouble(1, 175.5);
	if (database.executePrepared())
	{
		std::cout << "Successful insertion of user into table" << std::endl;
	}
	else
	{
		std::cout << "Unsuccessful insertion of user into table" << std::endl; 
	}
	//need to test all possible edge cases such as missing indices, repeated invokation of indices, missing binders
	//but the general proof that there is strong type safety stands

	//demonstrates recovery of future prepared statements following failure of previous
	database.prepareStatement("INSERT INTO Users (user_name, user_age, user_height) VALUES (?, ?, ?)", user_parameter_indices);
	database.bindString(1, "David");
	database.bindInt(2, 21);
	database.bindDouble(3, 166.6);
	if (database.executePrepared())
	{
		std::cout << "Successful insertion of user into table" << std::endl;
	}
	else
	{
		std::cout << "Unsuccessful insertion of user into table" << std::endl;
	}
}