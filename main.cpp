#include "DatabaseManager.hpp"

int main()
{
	DatabaseManager database("test_database.db");
	database.createTableIfNotExists
	(
		"Users",
		"user_id INT PRIMARY KEY AUTO_INCREMENT"
		"user_name VARCHAR(255) UNIQUE NOT NULL"
		"CONSTRAINT username_allowed_chars CHECK (username ~ '^[a-zA-Z0-9_]+$')"
		"user_salt VARCHAR(32) UNIQUE NOT NULL" 
	)
}