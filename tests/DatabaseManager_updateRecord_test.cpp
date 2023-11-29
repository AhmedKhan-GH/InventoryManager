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
        "user_timestamp     BIGINT    NOT NULL        DEFAULT CURRENT_TIMESTAMP"
    );

    /*
    database.createTableIfNotExists
    (
        "Logins",
        "login_id           INTEGER     PRIMARY KEY     AUTOINCREMENT, "
        "login_user         INTEGER     NOT NULL, "
        "FOREIGN KEY (login_user) REFERENCES Users(user_id), "
        "login_success      BOOLEAN     NOT NULL        DEFAULT 0, "
        "login_timestamp    DATETIME    NOT NULL        DEFAULT CURRENT_TIMESTAMP"
    );
    */

    UserDAO user_query(database);

    nlohmann::json user_data =
    {
        {"user_name", "test_user"},
        {"user_salt", "salt"},
        {"user_passhash", "hash"}
    };

    user_query.insertRecord(user_data);

    nlohmann::json user_dump = user_query.retrieveRecordById(1);
    std::cout << user_dump.dump();

    nlohmann::json new_user_data = 
    {
        {"user_name", "new_username"},
        {"user_salt", "pepper"},
        {"user_passhash", "newhash"},
        {"user_emailaddress", "email@email.com"}
    };

    user_query.updateRecordById(1, new_user_data);

    nlohmann::json updated_user = user_query.retrieveRecordById(1);

    std::cout << updated_user.dump() << std::endl;

}