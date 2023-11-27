#include "DatabaseManager.hpp"
#include "PasswordSecurity.hpp"
#include "UserDAO.hpp"
#include <iostream>
#include <map>

int main()
{
    DatabaseManager database("test_database.db");

    database.createTableIfNotExists
    (
        "Users",
        "user_id            INTEGER     PRIMARY KEY     AUTOINCREMENT, "
        "user_name          TEXT        NOT NULL        UNIQUE,"
        "user_salt          TEXT        NOT NULL, "
        "user_passhash      TEXT        NOT NULL, "
        "user_legalname     TEXT, "
        "user_phonenumber   TEXT, "
        "user_emailaddress  TEXT, "
        "user_description   TEXT, "
        "user_permission    INTEGER     NOT NULL        DEFAULT 1, "
        "user_visibility    BOOLEAN     NOT NULL        DEFAULT 1, "
        "user_timestamp     DATETIME    NOT NULL        DEFAULT CURRENT_TIMESTAMP"
    );


    UserDAO new_user(database);
    
    nlohmann::json user_data =
    {
        {"user_name", "john_doe"},
        {"user_salt", "abcd1234"}, 
        {"user_passhash", "xyz9876"}
    };

    std::cout << "json object created" << std::endl;
    
    if (new_user.insertRecord(user_data))
    {
        std::cout << "sucessfully inserted" << std::endl;
    }
    else
    {
        std::cout << "insertion failure" << std::endl;
    }
    
    nlohmann::json user_data2 =
    {
        {"user_name", "joe_schmoe"},
        {"user_salt", "abcd1234"},
        {"user_passhash", "xyz9876"},
        {"user_visibility", 1},
        {"user_permission", 2},
        {"user_timestamp", 1000000000}
    };

    UserDAO new_user2(database);
    std::cout << "json object created" << std::endl;

    if (new_user2.insertRecord(user_data2))
    {
        std::cout << "sucessfully inserted" << std::endl;
    }
    else
    {
        std::cout << "insertion failure" << std::endl;
    }
    
}