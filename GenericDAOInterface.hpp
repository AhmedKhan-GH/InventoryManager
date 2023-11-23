#ifndef GENERICDAOINTERFACE_HPP
#define GENERICDAOINTERFACE_HPP

#include "sqlite3.h"
#include <string>

class GenericDAOInterface
{
public:
    virtual ~GenericDAOInterface() {}

    // Insert a new record into the database
    virtual void insertRecord(const std::string& data) = 0;

    // Retrieve a record by its ID
    virtual std::string selectRecordById(int id) = 0;

    // Update an existing record by its ID
    virtual void updateRecordById(int id, const std::string& data) = 0;

    // Delete a record by its ID
    virtual void deleteRecordById(int id) = 0;
};

#endif GENERICDAOINTERFACE_HPP