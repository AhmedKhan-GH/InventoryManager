
#ifndef GENERICDAO_HPP
#define GENERICDAO_HPP

#include "sqlite3.h"
#include <string>
#include "nlohmann\\json.hpp"
#include "DatabaseManager.hpp"
#include <optional>

//generic dao can then be utilized by higher level logic classes with dependency injection
//and all derived classes are guaranteed by the interface to have the appropriate functions
//example, user logs in, upper classes call a generic dao insert to create a record
//or general queries to verify a login by comparing a login password to a stored hash
class GenericDAO
{
protected:

    enum class DataType
    {
        INTEGER,
        REAL,
        TEXT
    };

    explicit GenericDAO(DatabaseManager* _db_manager) : db_manager(_db_manager)
    {
        if (!db_manager) {
            throw std::invalid_argument("DatabaseManager cannot be null");
        }
    }

    virtual ~GenericDAO(){}

    //update individual column

    //C
    // Insert a new record into the database
    virtual bool insertRecord(const nlohmann::json& json_data) = 0;
    
    //R
    virtual nlohmann::json retrieveRecordById(int id) = 0;

    //U
    virtual bool updateRecordById(int id, nlohmann::json& json_data) = 0;
    
    //D
    // Delete a record by its ID
    virtual bool deleteRecordById(int id) = 0;

    /*
    //need queryFieldByID "get me the username of the user with this id"
    virtual bool existenceOfRecordByField(const std::string& field_name, const std::string& value) = 0;
    //"is there a user with this id or with this name etc"
    //need 

    virtual std::optional<std::string> queryFieldById(int id, const std::string& field_name) = 0;

    DatabaseManager* db_manager;
    */
};

#endif //GENERICDAO_HPP

