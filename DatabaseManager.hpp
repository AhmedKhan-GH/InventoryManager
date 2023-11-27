
#ifndef DATABASEMANAGER_HPP
#define DATABASEMANAGER_HPP
#include "Enums.hpp"

#include "sqlite3.h"
#include <string>
#include <stdexcept>
#include <iostream>
#include <map>



/// <summary>
/// Base level sqlite3 interface class
/// @date: 11/26/23
/// @author: Ahmed Khan
/// </summary>
class DatabaseManager {
public:

    enum class DataType
    {
        INTEGER,
        REAL,
        TEXT,
        // Add other data types as needed
    };

    DatabaseManager(const std::string& _database_path) : database_path(_database_path), statement_error(false) {
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
    bool prepareStatement(const std::string& sql, std::map<int, DataType>& _parameter_indices) {
        statement_error = false;
        if (sqlite3_prepare_v2(database_connection, sql.c_str(), -1, &prepared_statement, nullptr) != SQLITE_OK) {
            std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(database_connection) << std::endl;
            statement_error = true;
            return false;
        }

        parameter_indices = _parameter_indices;

        return true;
    }

    /// <summary>
    /// Binds a string value to a parameter in a prepared SQL statement.
    /// </summary>
    /// <param name="param_index">The index of the parameter to bind.</param>
    /// <param name="value">The string value to bind.</param>
    /// <returns>True if binding is successful, false otherwise.</returns>
    bool bindString(int param_index, const std::string& value) {
        return bindParameter<std::string>(param_index, value, DataType::TEXT);
    }

    /// <summary>
    /// Binds an integer value to a parameter in a prepared SQL statement.
    /// </summary>
    /// <param name="param_index">The index of the parameter to bind.</param>
    /// <param name="value">The integer value to bind.</param>
    /// <returns>True if binding is successful, false otherwise.</returns>
    bool bindInt(int param_index, const int value) {
        return bindParameter<int>(param_index, value, DataType::INTEGER);
    }

    /// <summary>
    /// Binds a double value to a parameter in a prepared SQL statement.
    /// </summary>
    /// <param name="param_index">The index of the parameter to bind.</param>
    /// <param name="value">The double value to bind.</param>
    /// <returns>True if binding is successful, false otherwise.</returns>
    bool bindDouble(int param_index, const double value) {
        return bindParameter<double>(param_index, value, DataType::REAL);
    }

    /// <summary>
    /// Binds a NULL value to a parameter in a prepared SQL statement.
    /// </summary>
    /// <param name="param_index">The index of the parameter to bind.</param>
    /// <returns>True if binding is successful, false otherwise.</returns>
    bool bindNull(int param_index) {
        if (statement_error) {
            std::cerr << "Error in bindNull: previous error prevents further modification" << std::endl;
            return false;
        }

        int result = sqlite3_bind_null(prepared_statement, param_index);
        if (result != SQLITE_OK) {
            std::cerr << "Error binding NULL to parameter index " << param_index << ": " << sqlite3_errmsg(database_connection) << std::endl;
            statement_error = true;
            return false;
        }

        return true;
    }

    
    /// <summary>
    /// Executes a prepared SQL statement.
    /// </summary>
    /// <returns>True if execution is successful, false otherwise.</returns>
    bool executePrepared() {
        if (statement_error == true)
        {
            std::cerr << "Error in executePrepared: previous error prevents futher modification" << std::endl;
            sqlite3_reset(prepared_statement);
            parameter_indices.clear();
            statement_error = false;
            return false;
        }
        if (sqlite3_step(prepared_statement) != SQLITE_DONE) {
            std::cerr << "Error in executePrepared: " << sqlite3_errmsg(database_connection) << std::endl;
            sqlite3_reset(prepared_statement);
            parameter_indices.clear();
            statement_error = false;
            return false;
        }
        //successful execution of prepared statement
        sqlite3_finalize(prepared_statement);
        parameter_indices.clear();
        statement_error = false;
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
    /// <summary>
    /// Binds a parameter in a prepared SQL statement to a specified value of type T.
    /// </summary>
    /// <typeparam name="T">The data type of the value to be bound (e.g., int, double, std::string).</typeparam>
    /// <param name="param_index">The index of the parameter to bind.</param>
    /// <param name="value">The value to bind to the parameter.</param>
    /// <param name="data_type">The expected data type of the parameter.</param>
    /// <returns>True if binding is successful, false otherwise.</returns>
    template <typename T>
    bool bindParameter(int param_index, const T& value, DataType expected_type) {
        
        if (statement_error == true)
        {
            std::cerr << "Error in bindParameter: previous error prevents futher modification" << std::endl;
            return false;
        }

        if (!checkParameterIndex(param_index, expected_type)) {
            return false;
        }

        int result = SQLITE_OK;

        if constexpr (std::is_same_v<T, std::string>) {
            result = sqlite3_bind_text(prepared_statement, param_index, value.c_str(), -1, SQLITE_TRANSIENT);
        }
        else if constexpr (std::is_same_v<T, int>) {
            result = sqlite3_bind_int(prepared_statement, param_index, value);
        }
        else if constexpr (std::is_same_v<T, double>) {
            result = sqlite3_bind_double(prepared_statement, param_index, value);
        }
        else {
            std::cerr << "Error: Unsupported data type" << std::endl;
            statement_error = true;
            return false;
        }

        if (result != SQLITE_OK) {
            statement_error = true;
            return false;
        }

        statement_error = false;
        return true;
    }
  
    /// <summary>
    /// Checks if a parameter at the specified index matches the expected data type.
    /// </summary>
    /// <param name="param_index">The index of the parameter to check.</param>
    /// <param name="expectedType">The expected data type of the parameter.</param>
    /// <returns>True if the parameter matches the expected type, false otherwise.</returns>
    bool checkParameterIndex(int param_index, DataType expectedType) {
        if (statement_error == true)
        {
            std::cerr << "Error: previous error prevents futher modification" << std::endl;
            return false;
        }

        if (parameter_indices.find(param_index) == parameter_indices.end()) {
            std::cerr << "Error: Parameter index " << param_index << " not found in the parameter_indices map." << std::endl;
            statement_error = true;
            return false;
        }

        DataType actualType = parameter_indices[param_index];
        if (actualType != expectedType) {
            std::cerr << "Error: Parameter at index " << param_index << " is not of the expected data type." << std::endl;
            statement_error = true;
            return false;
        }

        return true;
    }

    bool statement_error;
    std::map<int, DataType> parameter_indices;
    sqlite3* database_connection = nullptr;
    sqlite3_stmt* prepared_statement = nullptr;
    std::string database_path;
    friend class GenericDAO;

};


#endif //DATABASEMANAGER_HPP
