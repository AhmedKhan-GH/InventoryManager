#ifndef USERDAO_HPP
#define USERDAO_HPP

#include "DatabaseManager.hpp"
#include "GenericDAOInterface.hpp"
#include <string>
#include <stdexcept>

class UserDAO : public GenericDAOInterface {
public:
    UserDAO(DatabaseManager* db_manager) : db_manager(db_manager) {}

    void insertRecord(const std::string& data) override {
        // Assuming 'data' is a serialized string of user details
        // Parse 'data' and extract fields for user_name, user_salt, etc.
        // Example: parseData(data, user_name, user_salt, ...);

        try {
            std::string sql = "INSERT INTO Users (user_name, user_salt, user_passhash, user_legalname, "
                "user_phonenumber, user_emailaddress, user_description, user_permission) "
                "VALUES (?, ?, ?, ?, ?, ?, ?, ?);";
            db_manager->prepareStatement(sql);
            // Assuming bind parameters are set correctly according to the data
            db_manager->executePrepared();
        }
        catch (const std::runtime_error& e) {
            std::cerr << "Error inserting user: " << e.what() << std::endl;
            throw;
        }
    }

    std::string selectRecordById(int id) override {
        try {
            std::string sql = "SELECT * FROM Users WHERE user_id = ?;";
            db_manager->prepareStatement(sql);
            db_manager->bindParameter(1, id);
            db_manager->executePrepared();

            // Handle the query results
            // Example: return fetchResults();
            return ""; // Placeholder return

        }
        catch (const std::runtime_error& e) {
            std::cerr << "Error selecting user: " << e.what() << std::endl;
            throw;
        }
    }

    void updateRecordById(int id, const std::string& data) override {
        // Similar to insertRecord, parse 'data' for updated user details
        // Then prepare and execute an UPDATE SQL statement
    }

    void deleteRecordById(int id) override {
        try {
            std::string sql = "DELETE FROM Users WHERE user_id = ?;";
            db_manager->prepareStatement(sql);
            db_manager->bindParameter(1, id);
            db_manager->executePrepared();
        }
        catch (const std::runtime_error& e) {
            std::cerr << "Error deleting user: " << e.what() << std::endl;
            throw;
        }
    }

private:
    DatabaseManager* db_manager;
};

#endif // USERDAO_HPP