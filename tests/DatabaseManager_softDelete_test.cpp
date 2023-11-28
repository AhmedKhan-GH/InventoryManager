#include "DatabaseManager.hpp"
#include "PasswordSecurity.hpp"
#include <iostream>
#include <map>
#include "UserDAO.hpp"

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

    UserDAO user_insertion(database);

    nlohmann::json user_data =
    {
        {"user_name", "test_user"},
        {"user_salt", "salt"},
        {"user_passhash", "hash"}
    };

    user_insertion.insertRecord(user_data);

    user_insertion.deleteRecordById(1);
    //update of crud function should take a json of desired updates, comparing to a
    //map in userDAO defining what values can be edited and which are const
}