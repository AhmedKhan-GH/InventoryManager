
#ifndef GENERICDAO_HPP
#define GENERICDAO_HPP

#include "sqlite3.h"
#include <string>
#include "nlohmann\\json.hpp"
#include "DatabaseManager.hpp"
#include <optional>

class GenericDAO
{
protected:
    explicit GenericDAO(DatabaseManager* _db_manager) : db_manager(_db_manager)
    {
        if (!db_manager) {
            throw std::invalid_argument("DatabaseManager cannot be null");
        }
    }

    virtual ~GenericDAO(){}

    //update individual column

    // Insert a new record into the database
    virtual bool insertRecord(const nlohmann::json& json_data) = 0;

    // Delete a record by its ID
    virtual bool deleteRecordById(int id) = 0;

    void bindOptionalString(int bind_index, const std::string& key, const nlohmann::json& json_data, const std::optional<std::string>& default_value) {
        bindOptional<std::string>(json_data, key, bind_index, DatabaseManager::DataType::TEXT, default_value);
    }

    void bindOptionalInt(int bind_index, const std::string& key, const nlohmann::json& json_data, const std::optional<int>& default_value) {
        bindOptional<int>(json_data, key, bind_index, DatabaseManager::DataType::INTEGER, default_value);
    }

    void bindOptionalDouble(int bind_index, const std::string& key, const nlohmann::json& json_data, const std::optional<double>& default_value) {
        bindOptional<double>(json_data, key, bind_index, DatabaseManager::DataType::REAL, default_value);
    }

    void bindRequiredString(int bind_index, const std::string& key, const nlohmann::json& json_data) {
        db_manager->bindString(bind_index, json_data[key]);
    }

    void bindRequiredInt(int bind_index, const std::string& key, const nlohmann::json& json_data) {
        db_manager->bindInt(bind_index, json_data[key]);
    }

    void bindRequiredDouble(int bind_index, const std::string& key, const nlohmann::json& json_data) {
        db_manager->bindDouble(bind_index, json_data[key]);
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

    DatabaseManager* db_manager;

private:
    template<typename T>
    void bindOptional(const nlohmann::json& json_data, const std::string& key, int bind_index, DatabaseManager::DataType type, const std::optional<T>& default_value)
    {
        if (json_data.contains(key) && !json_data[key].is_null()) {
            T value = json_data[key].get<T>();
            db_manager->bindParameter<T>(bind_index, value, type);
        }
        else if (default_value.has_value()) {
            db_manager->bindParameter<T>(bind_index, default_value.value(), type);
        }
        else {
            db_manager->bindNull(bind_index);
        }
    }


};

#endif GENERICDAOE_HPP

