#ifndef DATABASEMANAGER_HPP
#define DATABASEMANAGER_HPP

#include "sqlite3.h"
#include <string>
#include <stdexcept>
#include <iostream>

/// <summary>
/// Base level sqlite3 interface class
/// @date: 11/26/23
/// @author: Ahmed Khan
/// </summary>
class DatabaseManager {
public:
    DatabaseManager(const std::string& _database_path) : database_path(_database_path) {
        if (sqlite3_open(database_path.c_str(), &database_connection) != SQLITE_OK) {
            std::cerr << "Failed to open database: " << sqlite3_errmsg(database_connection) << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    
    ~DatabaseManager() {
        sqlite3_close(database_connection);
    }

    /// <summary>
    /// Creates a database table if it does not already exist in the database.
    /// EXITS PROGRAM IF GIVEN IMPROPER SCHEMA FORMATTING
    /// </summary>
    /// <param name="table_name">name of table</param>
    /// <param name="table_schema">schema definition using sqlite3 formatting</param>
    void createTableIfNotExists(const std::string& table_name, const std::string& table_schema) {
        std::string create_table_sql = "CREATE TABLE IF NOT EXISTS " + table_name + " (" + table_schema + ");";
        executeQuery(create_table_sql);
    }

    /// <summary>
    /// Executes an SQL query on the database.
    /// </summary>
    /// <param name="query">The SQL query to be executed.</param>
    void executeQuery(const std::string& query) {
        char* error_message = nullptr;
        if (sqlite3_exec(database_connection, query.c_str(), nullptr, nullptr, &error_message) != SQLITE_OK) {
            std::cerr << "Error in executeQuery: " << error_message << std::endl;
            sqlite3_free(error_message);
            sqlite3_close(database_connection);
            exit(EXIT_FAILURE);
        }
    }

    /// <summary>
    /// Prepares an SQL statement for execution.
    /// </summary>
    /// <param name="sql">The SQL statement to be prepared.</param>
    /// <returns>True if preparation is successful, false otherwise.</returns>
    bool prepareStatement(const std::string& sql) {
        if (sqlite3_prepare_v2(database_connection, sql.c_str(), -1, &prepared_statement, nullptr) != SQLITE_OK) {
            std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(database_connection) << std::endl;
            return false;
        }
        return true;
    }

    /// <summary>
    /// Binds a string value to a parameter in a prepared SQL statement.
    /// </summary>
    /// <param name="paramIndex">The index of the parameter to bind.</param>
    /// <param name="value">The string value to bind.</param>
    /// <returns>True if binding is successful, false otherwise.</returns>
    bool bindString(int paramIndex, const std::string& value) {
        if (sqlite3_bind_text(prepared_statement, paramIndex, value.c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK) {
            std::cerr << "Error in bindString: " << sqlite3_errmsg(database_connection) << std::endl;
            sqlite3_finalize(prepared_statement); // Finalize the prepared statement to clear error state
            return false;
        }
        return true;
    }

    /// <summary>
    /// Binds an integer value to a parameter in a prepared SQL statement.
    /// </summary>
    /// <param name="paramIndex">The index of the parameter to bind.</param>
    /// <param name="value">The integer value to bind.</param>
    /// <returns>True if binding is successful, false otherwise.</returns>
    bool bindInt(int paramIndex, int value) {
        if (sqlite3_bind_int(prepared_statement, paramIndex, value) != SQLITE_OK) {
            std::cerr << "Error in bindInt: " << sqlite3_errmsg(database_connection) << std::endl;
            sqlite3_finalize(prepared_statement); // Finalize the prepared statement to clear error state
            return false;
        }
        return true;
    }

    /// <summary>
    /// Binds a double value to a parameter in a prepared SQL statement.
    /// </summary>
    /// <param name="paramIndex">The index of the parameter to bind.</param>
    /// <param name="value">The double value to bind.</param>
    /// <returns>True if binding is successful, false otherwise.</returns>
    bool bindDouble(int paramIndex, double value) {
        if (sqlite3_bind_double(prepared_statement, paramIndex, value) != SQLITE_OK) {
            std::cerr << "Error in bindDouble: " << sqlite3_errmsg(database_connection) << std::endl;
            sqlite3_finalize(prepared_statement); // Finalize the prepared statement to clear error state
            return false;
        }
        return true;
    }


    /// <summary>
    /// Executes a prepared SQL statement.
    /// </summary>
    /// <returns>True if execution is successful, false otherwise.</returns>
    bool executePrepared() {
        if (sqlite3_step(prepared_statement) != SQLITE_DONE) {
            std::cerr << "Error in executePrepared: " << sqlite3_errmsg(database_connection) << std::endl;
            sqlite3_finalize(prepared_statement);
            return false;
        }
        sqlite3_finalize(prepared_statement);
        return true;
    }

    /// <summary>
    /// Gets the SQLite3 prepared statement that has been previously prepared.
    /// </summary>
    /// <returns>The SQLite3 prepared statement.</returns>
    sqlite3_stmt* getPreparedStatement() {
        return prepared_statement;
    }

private:
    sqlite3* database_connection = nullptr;
    sqlite3_stmt* prepared_statement = nullptr;
    std::string database_path;
};

#endif //DATABASEMANAGER_HPP