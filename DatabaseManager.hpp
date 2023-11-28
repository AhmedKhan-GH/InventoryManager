
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

    void createTableIfNotExists(const std::string& table_name, const std::string& table_schema) {
        std::string create_table_sql = "CREATE TABLE IF NOT EXISTS " + table_name + " (" + table_schema + ");";
        executeQuery(create_table_sql);
    }

    void executeQuery(const std::string& query) {
        char* error_message = nullptr;
        if (sqlite3_exec(database_connection, query.c_str(), nullptr, nullptr, &error_message) != SQLITE_OK) {
            std::cerr << "Error in executeQuery: " << error_message << std::endl;
            sqlite3_free(error_message);
            sqlite3_close(database_connection);
            exit(EXIT_FAILURE);
        }
    }

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

    bool bindString(int param_index, const std::string& value) {
        return bindParameter<std::string>(param_index, value, DataType::TEXT);
    }

    bool bindInt(int param_index, const int value) {
        return bindParameter<int>(param_index, value, DataType::INTEGER);
    }

    bool bindDouble(int param_index, const double value) {
        return bindParameter<double>(param_index, value, DataType::REAL);
    }    

    void bindJsonOptionalString(int bind_index, const std::string& key, const nlohmann::json& json_data, const std::optional<std::string>& default_value) {
        bindOptional<std::string>(json_data, key, bind_index, DataType::TEXT, default_value);
    }

    void bindJsonOptionalInt(int bind_index, const std::string& key, const nlohmann::json& json_data, const std::optional<int>& default_value) {
        bindOptional<int>(json_data, key, bind_index, DataType::INTEGER, default_value);
    }

    void bindJsonOptionalDouble(int bind_index, const std::string& key, const nlohmann::json& json_data, const std::optional<double>& default_value) {
        bindOptional<double>(json_data, key, bind_index, DataType::REAL, default_value);
    }

    void bindJsonRequiredString(int bind_index, const std::string& key, const nlohmann::json& json_data) {
        bindParameter<std::string>(bind_index, json_data[key], DataType::TEXT);
    }

    void bindJsonRequiredInt(int bind_index, const std::string& key, const nlohmann::json& json_data) {
        bindParameter<int>(bind_index, json_data[key], DataType::INTEGER);
    }

    void bindJsonRequiredDouble(int bind_index, const std::string& key, const nlohmann::json& json_data) {
        bindParameter<double>(bind_index, json_data[key], DataType::REAL);
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

    void getSelectString(int index, sqlite3_stmt* prepared_statement, std::string field, nlohmann::json& json_result)
    {
        std::optional<std::string> result = getSelectParameter<std::string>(prepared_statement, index);
        if (result.has_value())
        {
            json_result[field] = result.value();
        }
        return;
    }

    void getSelectDouble(int index, sqlite3_stmt* prepared_statement, std::string field, nlohmann::json& json_result)
    {
        std::optional<double> result = getSelectParameter<double>(prepared_statement, index);
        if (result.has_value())
        {
            json_result[field] = result.value();
        }
        return;
    }

    void getSelectInt(int index, sqlite3_stmt* prepared_statement, std::string field, nlohmann::json& json_result)
    {
        std::optional<int> result = getSelectParameter<int>(prepared_statement, index);
        if (result.has_value())
        {
            json_result[field] = result.value();
        }
        return;
    }

    bool validateJsonFields
    (
        const nlohmann::json& json_data,
        const std::map<std::string, nlohmann::json::value_t>& required_fields
    )
    {
        for (const auto& field : required_fields)
        {
            if (!json_data.contains(field.first) || json_data[field.first].type() != field.second)
            {
                std::cerr << "Error in validateJsonFields: Missing or incorrect required JSON field: " << field.first << std::endl;
                return false;
            }
        }
        return true;
    }

    sqlite3_stmt* getPreparedStatement() {
        return prepared_statement;
    }

private:

    template<typename T>
    std::optional<T> getSelectParameter(sqlite3_stmt* prepared_statement, int columnIndex) {
        std::cerr << "Error in getSelectParameter: unsupported type." << std::endl;
        return std::nullopt; // Unsupported type
    }

    template<>
    std::optional<int> getSelectParameter<int>(sqlite3_stmt* prepared_statement, int columnIndex) {
        if (sqlite3_column_type(prepared_statement, columnIndex) == SQLITE_NULL) {
            return std::nullopt;
        }
        return sqlite3_column_int(prepared_statement, columnIndex);
    }

    // Specialization for double
    template<>
    std::optional<double> getSelectParameter<double>(sqlite3_stmt* prepared_statement, int columnIndex) {
        if (sqlite3_column_type(prepared_statement, columnIndex) == SQLITE_NULL) {
            return std::nullopt;
        }
        return sqlite3_column_double(prepared_statement, columnIndex);
    }

    // Specialization for string
    template<>
    std::optional<std::string> getSelectParameter<std::string>(sqlite3_stmt* prepared_statement, int columnIndex) {
        if (sqlite3_column_type(prepared_statement, columnIndex) == SQLITE_NULL) {
            return std::nullopt;
        }
        const char* data = reinterpret_cast<const char*>(sqlite3_column_text(prepared_statement, columnIndex));
        return data ? std::optional<std::string>(data) : std::nullopt;
    }

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

    template<typename T>
    void bindOptional(const nlohmann::json& json_data, const std::string& key, int bind_index, DataType type, const std::optional<T>& default_value)
    {
        if (json_data.contains(key) && !json_data[key].is_null()) {
            T value = json_data[key].get<T>();
            bindParameter<T>(bind_index, value, type);
            return; // Guard clause to exit early
        }

        if (default_value.has_value()) {
            bindParameter<T>(bind_index, default_value.value(), type);
            return; // Guard clause to exit early
        }

        bindNull(bind_index);

    }


    bool statement_error;
    std::map<int, DataType> parameter_indices;
    sqlite3* database_connection = nullptr;
    sqlite3_stmt* prepared_statement = nullptr;
    std::string database_path;
    friend class GenericDAO;

};


#endif //DATABASEMANAGER_HPP
