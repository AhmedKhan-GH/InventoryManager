#ifndef GENERICDAOINTERFACE_HPP
#define GENERICDAOINTERFACE_HPP

#include "sqlite3.h"
#include <string>

class GenericDAOInterface
{
public:
	virtual ~GenericDAOInterface() {}
    
    // Create operation
    virtual void create(const std::string & data) = 0;

    // Read operation
    virtual std::string read(int id) = 0;

    // Update operation
    virtual void update(int id, const std::string & data) = 0;

    // Delete operation
    virtual void delete_(int id) = 0;

};

#endif GENERICDAOINTERFACE_HPP