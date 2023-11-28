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

}