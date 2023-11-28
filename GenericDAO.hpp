
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

    virtual nlohmann::json retrieveRecordById(int id) = 0;

    virtual bool updateRecordByID(int id) = 0;

    //need queryFieldByID "get me the username of the user with this id"
    //need existenceOfRecordByField (including id) "is there a user with this id or with this name etc"
    //need 

    DatabaseManager* db_manager;

};

#endif GENERICDAOE_HPP

