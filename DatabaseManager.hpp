#ifndef DATABASEMANAGER_HPP
#define DATABASEMANAGER_HPP

#include "sqlite3.h"
#include <string>
#include <stdexcept>
#include <iostream>

class DatabaseManager {
public:
    DatabaseManager(const std::string& database_path) : database_path(database_path) {
        if (sqlite3_open(database_path.c_str(), &database_connection) != SQLITE_OK) {
            std::string error_message = "Failed to open database: ";
            error_message += sqlite3_errmsg(database_connection);
            throw std::runtime_error(error_message);
        }
    }
    
    ~DatabaseManager() {
        sqlite3_close(database_connection);
    }

    void createTableIfNotExists(const std::string& table_name, const std::string& table_schema) {
        std::string create_table_sql = "CREATE TABLE IF NOT EXISTS " + table_name + " (" + table_schema + ");";
        executeQuery(create_table_sql);
    }

    void executeQuery(const std::string& query) {
        char* error_message = nullptr;
        if (sqlite3_exec(database_connection, query.c_str(), nullptr, nullptr, &error_message) != SQLITE_OK) {
            std::string error = "Error in executeQuery: ";
            error += error_message;
            sqlite3_free(error_message);
            throw std::runtime_error(error);
        }
    }

    void prepareStatement(const std::string& sql) {
        if (sqlite3_prepare_v2(database_connection, sql.c_str(), -1, &prepared_statement, nullptr) != SQLITE_OK) {
            std::string error_message = "Failed to prepare statement: ";
            error_message += sqlite3_errmsg(database_connection);
            throw std::runtime_error(error_message);
        }
    }

    void bindString(int paramIndex, const std::string& value) {
        if (sqlite3_bind_text(prepared_statement, paramIndex, value.c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK) {
            throw std::runtime_error(sqlite3_errmsg(database_connection));
        }
    }

    void bindInt(int paramIndex, int value) {
        if (sqlite3_bind_int(prepared_statement, paramIndex, value) != SQLITE_OK) {
            throw std::runtime_error(sqlite3_errmsg(database_connection));
        }
    }

    void bindDouble(int paramIndex, double value) {
        if (sqlite3_bind_double(prepared_statement, paramIndex, value) != SQLITE_OK) {
            throw std::runtime_error(sqlite3_errmsg(database_connection));
        }
    }

    void executePrepared() {
        if (sqlite3_step(prepared_statement) != SQLITE_DONE) {
            throw std::runtime_error(sqlite3_errmsg(database_connection));
        }
        sqlite3_finalize(prepared_statement);
    }

    sqlite3_stmt* getPreparedStatement() {
        return prepared_statement;
    }

private:
    sqlite3* database_connection = nullptr;
    sqlite3_stmt* prepared_statement = nullptr;
    std::string database_path;
};

#endif DATABASEMANAGER_HPP
