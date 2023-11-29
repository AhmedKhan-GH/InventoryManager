
#ifndef DATABASEMANAGER_HPP
#define DATABASEMANAGER_HPP

#include "sqlite3.h"
#include <string>
#include <stdexcept>
#include <iostream>
#include <map>
#include "nlohmann\\json.hpp"

/// <summary>
/// Base level sqlite3 interface class
/// @date: 11/26/23
/// @author: Ahmed Khan
/// </summary>
class DatabaseManager {
public:


    //Constructor
    DatabaseManager(const std::string& _database_path) : database_path(_database_path), statement_error(false) {
        if (sqlite3_open(database_path.c_str(), &database_connection) != SQLITE_OK) {
            std::cerr << "Failed to open database: " << sqlite3_errmsg(database_connection) << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    
    //Destructor
    ~DatabaseManager() {
        sqlite3_close(database_connection);
    }

    // Basic Operations --------------------------------------------------------------------------------------

    void executeQuery(const std::string& query) {
        char* error_message = nullptr;
        if (sqlite3_exec(database_connection, query.c_str(), nullptr, nullptr, &error_message) != SQLITE_OK) {
            std::cerr << "Error in executeQuery: " << error_message << std::endl;
            sqlite3_free(error_message);
            sqlite3_close(database_connection);
            exit(EXIT_FAILURE);
        }
    }

    void createTableIfNotExists(const std::string& table_name, const std::string& table_schema) {
        std::string create_table_sql = "CREATE TABLE IF NOT EXISTS " + table_name + " (" + table_schema + ");";
        executeQuery(create_table_sql);
    }

    // Prepared Statements ----------------------------------------------------------------------------------

    bool prepareStatement(const std::string& sql) {
        statement_error = false;
        if (sqlite3_prepare_v2(database_connection, sql.c_str(), -1, &prepared_statement, nullptr) != SQLITE_OK) {
            std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(database_connection) << std::endl;
            statement_error = true;
            return false;
        }

        return true;
    }

    bool executePrepared() {
        if (statement_error == true)
        {
            std::cerr << "Error in executePrepared: previous error prevents futher modification" << std::endl;
            sqlite3_reset(prepared_statement);
            statement_error = false;
            return false;
        }
        if (sqlite3_step(prepared_statement) != SQLITE_DONE) {
            std::cerr << "Error in executePrepared: " << sqlite3_errmsg(database_connection) << std::endl;
            sqlite3_reset(prepared_statement);
            statement_error = false;
            return false;
        }
        //successful execution of prepared statement
        sqlite3_finalize(prepared_statement);
        statement_error = false;
        return true;
    }


    sqlite3_stmt* getPreparedStatement() {
        return prepared_statement;
    }

    // Parameter Bindings ----------------------------------------------------------------------------------

    template <typename T>
    bool bindParameter(int param_index, const T& value) {

        if (statement_error == true)
        {
            std::cerr << "Error in bindParameter: previous error prevents futher modification" << std::endl;
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

    template<typename T>
    void bindOptional(int bind_index, const nlohmann::json& json_data, const std::string& key, const std::optional<T>& default_value)
    {
        if (json_data.contains(key) && !json_data[key].is_null()) {
            T value = json_data[key].get<T>();
            bindParameter<T>(bind_index, value);
            return; // Guard clause to exit early
        }

        if (default_value.has_value()) {
            bindParameter<T>(bind_index, default_value.value());
            return; // Guard clause to exit early
        }

        bindNull(bind_index);

    }

    template<typename T>
    void getParameter(int index, nlohmann::json& json_result, const std::string& field, sqlite3_stmt* prepared_statement) {
        // Check for null value first
        if (sqlite3_column_type(prepared_statement, index) == SQLITE_NULL) {
            return; // No value to assign
        }

        // Retrieve and assign value based on type T
        if constexpr (std::is_same_v<T, int>) {
            json_result[field] = sqlite3_column_int(prepared_statement, index);
        }
        else if constexpr (std::is_same_v<T, double>) {
            json_result[field] = sqlite3_column_double(prepared_statement, index);
        }
        else if constexpr (std::is_same_v<T, std::string>) {
            const char* data = reinterpret_cast<const char*>(sqlite3_column_text(prepared_statement, index));
            if (data) {
                json_result[field] = std::string(data);
            }
        }
    }

    bool fetchBooleanResult() {
        if (sqlite3_step(prepared_statement) == SQLITE_ROW) {
            bool result = sqlite3_column_int(prepared_statement, 0) != 0;
            sqlite3_finalize(prepared_statement);
            return result;
        }
        sqlite3_finalize(prepared_statement);
        return false;
    }


private:

    bool statement_error;
    sqlite3* database_connection = nullptr;
    sqlite3_stmt* prepared_statement = nullptr;
    std::string database_path;
    friend class GenericDAO;

};


#endif //DATABASEMANAGER_HPP
