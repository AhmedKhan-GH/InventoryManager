#ifndef GENERICDAO_HPP
#define GENERICDAO_HPP

#include "sqlite3.h"
#include <string>
#include "nlohmann\\json.hpp"

class GenericDAO
{
public:
    virtual ~GenericDAO() {}

    // Insert a new record into the database
    virtual void insertRecord(const std::string& data) = 0;

    // Retrieve a record by its ID
    virtual nlohmann::json selectRecordById(int id) = 0;

    virtual void updateStringById(int id, const std::string& columnName, const std::string& newValue) = 0;

    virtual void updateIntById(int id, const std::string& columnName, int newValue) = 0;

    virtual void updateDoubleById(int id, const std::string& columnName, double newValue) = 0;

    // Delete a record by its ID
    virtual void deleteRecordById(int id) = 0;
};

#endif GENERICDAOE_HPP