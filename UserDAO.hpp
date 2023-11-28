
#ifndef USERDAO_HPP
#define USERDAO_HPP

#include "DatabaseManager.hpp"
#include "GenericDAO.hpp"
#include <string>
#include <stdexcept>
#include "nlohmann\\json.hpp"
#include <chrono>

class UserDAO : public GenericDAO {
public:

    UserDAO(DatabaseManager &db_manager) : GenericDAO(&db_manager) {}

    //the C in CRUD
    bool insertRecord(const nlohmann::json& json_data)
    {
        //sql query stays as normal
        std::string parameter_insert =
            "INSERT INTO Users (user_name, user_salt, user_passhash, user_legalname, user_phonenumber, "
            "user_emailaddress, user_description, user_permission, user_visibility, user_timestamp) "
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";

        db_manager->prepareStatement(parameter_insert);

        //required bindings already validated
        db_manager->bindParameter<std::string>(1, json_data["user_name"]);
        db_manager->bindParameter<std::string>(2, json_data["user_salt"]);
        db_manager->bindParameter<std::string>(3, json_data["user_passhash"]);

        db_manager->bindOptional<std::string>(4, json_data, "user_legalname", std::nullopt);
        db_manager->bindOptional<std::string>(5, json_data, "user_phonenumber", std::nullopt);
        db_manager->bindOptional<std::string>(6, json_data, "user_emailaddress", std::nullopt);
        db_manager->bindOptional<std::string>(7, json_data, "user_description", std::nullopt);

        db_manager->bindOptional<int>(8, json_data, "user_permission", 1);
        db_manager->bindOptional<int>(9, json_data, "user_visibility", 1);
        db_manager->bindOptional<int>(10, json_data, "user_timestamp", std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());

        if (!db_manager->executePrepared())
        {
            std::cerr << "Error in insertRecord" << std::endl;
            return false;
        }

        return true;
    }

    
    //R in CRUD, Retrieve record by ID
    nlohmann::json retrieveRecordById(int id) override
    {
        nlohmann::json json_result;
   
        std::string sql = "SELECT * FROM Users WHERE user_id = ?;";
        db_manager->prepareStatement(sql);
        db_manager->bindParameter<int>(1, id);
        sqlite3_stmt* prepared_statement = db_manager->getPreparedStatement();

        if (sqlite3_step(prepared_statement) == SQLITE_ROW) {
            // Assuming column indices are in order as per your table schema

            db_manager->getSelectAndAssign<std::string>(1, prepared_statement, "user_name", json_result);
            db_manager->getSelectAndAssign<std::string>(2, prepared_statement, "user_salt", json_result);
            db_manager->getSelectAndAssign<std::string>(3, prepared_statement, "user_passhash", json_result);
            db_manager->getSelectAndAssign<std::string>(4, prepared_statement, "user_legalname", json_result);
            db_manager->getSelectAndAssign<std::string>(5, prepared_statement, "user_phonenumber", json_result);
            db_manager->getSelectAndAssign<std::string>(6, prepared_statement, "user_emailaddress", json_result);
            db_manager->getSelectAndAssign<std::string>(7, prepared_statement, "user_description", json_result);

            db_manager->getSelectAndAssign<int>(8, prepared_statement, "user_permission", json_result);
            db_manager->getSelectAndAssign<int>(9, prepared_statement, "user_visibility", json_result);
            db_manager->getSelectAndAssign<int>(10, prepared_statement, "user_timestamp", json_result);
        }

        return json_result;
    }


    //U in CRUD, Update allowed paramters by ID



    /*
    nlohmann::json selectRecordById(int id) {
       
        try {
            std::string sql = "SELECT * FROM Users WHERE user_id = ?;";
            db_manager->prepareStatement(sql);
            db_manager->bindInt(1, id);

            if (sqlite3_step(db_manager->getPreparedStatement()) == SQLITE_ROW) {
                nlohmann::json json_result;
                json_result["user_id"] = getIntFromColumn(0);
                json_result["user_name"] = getStringFromColumn(1);
                json_result["user_salt"] = getStringFromColumn(2);
                json_result["user_passhash"] = getStringFromColumn(3);
                json_result["user_timestamp"] = getStringFromColumn(4);
                json_result["user_visibility"] = getIntFromColumn(5);
                json_result["user_legalname"] = getStringFromColumn(6);
                json_result["user_phonenumber"] = getStringFromColumn(7);
                json_result["user_emailaddress"] = getStringFromColumn(8);
                json_result["user_description"] = getStringFromColumn(9);
                json_result["user_permission"] = getStringFromColumn(10);

                sqlite3_finalize(db_manager->getPreparedStatement());
                return json_result;
            }
            else {
                sqlite3_finalize(db_manager->getPreparedStatement());
                throw std::runtime_error("No record found");
            }
        }
        catch (const std::runtime_error& e) {
            std::cerr << "Error selecting user: " << e.what() << std::endl;
            throw;
        }
   
    }


    std::string getStringFromColumn(int columnIndex) {
        const unsigned char* text = sqlite3_column_text(db_manager->getPreparedStatement(), columnIndex);
        if (text != nullptr) {
            return std::string(reinterpret_cast<const char*>(text));
        }
        else {
            return ""; // or handle null value appropriately
        }
    }

    // Updated getIntFromColumn with a default value
    int getIntFromColumn(int columnIndex, int defaultValue = 0) {
        try {
            if (sqlite3_column_type(db_manager->getPreparedStatement(), columnIndex) == SQLITE_NULL) {
                return defaultValue;
            }
            return sqlite3_column_int(db_manager->getPreparedStatement(), columnIndex);
        }
        catch (const std::exception& e) {
            std::cerr << "Error fetching int: " << e.what() << std::endl;
            return defaultValue; // Return default value in case of error
        }
    }

    // Updated getDoubleFromColumn with a default value
    double getDoubleFromColumn(int columnIndex, double defaultValue = 0.0) {
        try {
            if (sqlite3_column_type(db_manager->getPreparedStatement(), columnIndex) == SQLITE_NULL) {
                return defaultValue;
            }
            return sqlite3_column_double(db_manager->getPreparedStatement(), columnIndex);
        }
        catch (const std::exception& e) {
            std::cerr << "Error fetching double: " << e.what() << std::endl;
            return defaultValue; // Return default value in case of error
        }

    }

    void updateStringById(int id, const std::string& columnName, const std::string& newValue) {
        std::string sql = "UPDATE Users SET " + columnName + " = ? WHERE user_id = ?;";
        db_manager->prepareStatement(sql);
        db_manager->bindString(1, newValue);
        db_manager->bindInt(2, id);
        db_manager->executePrepared();
    }

    void updateIntById(int id, const std::string& columnName, int newValue) {
        std::string sql = "UPDATE Users SET " + columnName + " = ? WHERE user_id = ?;";
        db_manager->prepareStatement(sql);
        db_manager->bindInt(1, newValue);
        db_manager->bindInt(2, id);
        db_manager->executePrepared();
    }

    void updateDoubleById(int id, const std::string& columnName, double newValue) {
        std::string sql = "UPDATE Users SET " + columnName + " = ? WHERE user_id = ?;";
        db_manager->prepareStatement(sql);
        db_manager->bindDouble(1, newValue);
        db_manager->bindInt(2, id);
        db_manager->executePrepared();
    }
    */
    
    //D in CRUD
    bool deleteRecordById(int id) override {
        std::string sql = "UPDATE Users SET user_visibility = ? WHERE user_id = ?;";
        db_manager->prepareStatement(sql);
        db_manager->bindParameter<int>(1, 0);
        db_manager->bindParameter<int>(2, id);
        if(!db_manager->executePrepared())
        {
            std::cerr << "Error in deleteRecordById." << std::endl;
            return false;
        }
        return true;
    }
};

#endif // USERDAO_HPP

/*
   std::string delete_sql = "DELETE FROM Users WHERE user_id = ?;";
        std::map<int, DatabaseManager::DataType> delete_parameter = { { 1, DatabaseManager::DataType::INTEGER } };

        db_manager->prepareStatement(delete_sql, delete_parameter);
        db_manager->bindInt(1, id);
        if (!db_manager->executePrepared())
        {
            std::cerr << "Error deleting user." << std::endl;
        }
*/
