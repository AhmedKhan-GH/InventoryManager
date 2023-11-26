#ifndef DATABASEMANAGER_HPP
#define DATABASEMANAGER_HPP

#include "sqlite3.h"
#include <string>
#include <stdexcept>
#include <iostream>

class DatabaseManager {
public:
    DatabaseManager(const std::string& _database_path) : database_path(_database_path) {
        if (sqlite3_open(database_path.c_str(), &database_connection) != SQLITE_OK) {
            std::string error_message = "Failed to open database: ";
            error_message += sqlite3_errmsg(database_connection);
            throw std::runtime_error(error_message);
        }
    }
    
    ~DatabaseManager() {
        sqlite3_close(database_connection);
    }

    /// <summary>
    /// Creates a database table if it does not already exist in the database.
    /// </summary>
    /// <param name="table_name">The name of the table to create or check for existence.</param>
    /// <param name="table_schema">The schema definition of the table, including column definitions.</param>
    void createTableIfNotExists(const std::string& table_name, const std::string& table_schema) {
        try {
            // Construct the SQL query
            std::string create_table_sql = "CREATE TABLE IF NOT EXISTS " + table_name + " (" + table_schema + ");";

            executeQuery(create_table_sql);
        }
        catch (const std::runtime_error& e) {
            // Handle and propagate any exceptions that occur during table creation
            std::cerr << "Exception caught in createTableIfNotExists: " << e.what() << std::endl;
        }
    }

    // executeQuery
    void executeQuery(const std::string& query) {
        char* error_message = nullptr;
        try {
            if (sqlite3_exec(database_connection, query.c_str(), nullptr, nullptr, &error_message) != SQLITE_OK) {
                std::string error = "Error in executeQuery: ";
                error += error_message;
                throw std::runtime_error(error);
            }
        }
        catch (const std::runtime_error& e) {
            sqlite3_free(error_message);
            std::cerr << "Exception caught in executeQuery: " << e.what() << std::endl;
        }
    }

    void prepareStatement(const std::string& sql) {
        try {
            if (sqlite3_prepare_v2(database_connection, sql.c_str(), -1, &prepared_statement, nullptr) != SQLITE_OK) {
                throw std::runtime_error("Failed to prepare statement: " + std::string(sqlite3_errmsg(database_connection)));
            }
        }
        catch (const std::runtime_error& e) {
            std::cerr << "Exception caught in prepareStatement: " << e.what() << std::endl;
            throw;
        }
    }

    void bindString(int paramIndex, const std::string& value) {
        try {
            if (sqlite3_bind_text(prepared_statement, paramIndex, value.c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK) {
                throw std::runtime_error("Error in bindString: " + std::string(sqlite3_errmsg(database_connection)));
            }
        }
        catch (const std::runtime_error& e) {
            std::cerr << "Exception caught in bindString: " << e.what() << std::endl;
            throw;
        }
    }

    void bindInt(int paramIndex, int value) {
        try {
            if (sqlite3_bind_int(prepared_statement, paramIndex, value) != SQLITE_OK) {
                throw std::runtime_error("Error in bindInt: " + std::string(sqlite3_errmsg(database_connection)));
            }
        }
        catch (const std::runtime_error& e) {
            std::cerr << "Exception caught in bindInt: " << e.what() << std::endl;
            throw;
        }
    }

    void bindDouble(int paramIndex, double value) {
        try {
            if (sqlite3_bind_double(prepared_statement, paramIndex, value) != SQLITE_OK) {
                throw std::runtime_error("Error in bindDouble: " + std::string(sqlite3_errmsg(database_connection)));
            }
        }
        catch (const std::runtime_error& e) {
            std::cerr << "Exception caught in bindDouble: " << e.what() << std::endl;
            throw;
        }
    }

    void executePrepared() {
        try {
            if (sqlite3_step(prepared_statement) != SQLITE_DONE) {
                throw std::runtime_error("Error in executePrepared: " + std::string(sqlite3_errmsg(database_connection)));
            }
        }
        catch (const std::runtime_error& e) {
            sqlite3_finalize(prepared_statement);
            std::cerr << "Exception caught in executePrepared: " << e.what() << std::endl;
            throw;
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
