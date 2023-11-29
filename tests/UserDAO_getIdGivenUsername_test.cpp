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

    database.createTableIfNotExists
    (
        "Logins",
        "login_id           INTEGER     PRIMARY KEY     AUTOINCREMENT, "
        "login_user         INTEGER     NOT NULL, "
        "FOREIGN KEY (login_user) REFERENCES Users(user_id), "
        "login_success      BOOLEAN     NOT NULL        DEFAULT 0, "
        "login_timestamp    DATETIME    NOT NULL        DEFAULT CURRENT_TIMESTAMP"
    );


    UserDAO user_data_object(database);

    nlohmann::json user_data =
    {
        {"user_name", "test_user"},
        {"user_passhash", "hash"},
        {"user_salt", "salt"}
    };


    user_data_object.insertRecord(user_data);

    std::cout << user_data_object.retrieveRecordById(1).dump() << std::endl;

    user_data["user_name"] = "newer_name";

    user_data_object.insertRecord(user_data);

    std::cout << user_data_object.retrieveRecordById(2).dump() << std::endl;


    std::cout << user_data_object.getIdGivenUsername("newer_name").value_or(-1) << std::endl;
}