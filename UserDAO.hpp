
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

    enum class UserPermission
    {
        LOCK,
        BASE,
        SUPER,
        ADMIN
    };

    UserDAO(DatabaseManager &db_manager) : GenericDAO(&db_manager) {}

    //the C in CRUD
    bool insertRecord(const nlohmann::json& json_data)
    {
        //encapsulate this map of strings and json value_t into a single RequiredFileds object that can be given a bracketized array of strings and custom enums
        const std::map<std::string, nlohmann::json::value_t> required_fields =
        {
            {"user_name", nlohmann::json::value_t::string},
            {"user_salt", nlohmann::json::value_t::string},
            {"user_passhash", nlohmann::json::value_t::string}
        };

        //
        if (!GenericDAO::validateJsonFields(json_data, required_fields))
        {
            std::cerr << "Error in insertRecord: JSON entries invalid" << std::endl;
            return false;
        }

        //sql query stays as normal
        std::string parameter_insert =
            "INSERT INTO Users (user_name, user_salt, user_passhash, user_legalname, user_phonenumber, "
            "user_emailaddress, user_description, user_permission, user_visibility, user_timestamp) "
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";

        //do the same encapsulation of map of int and DataType enums into a single object
        std::map<int, DatabaseManager::DataType> parameter_types =
        {
            {1, DatabaseManager::DataType::TEXT},    //user_name
            {2, DatabaseManager::DataType::TEXT},    //user_salt
            {3, DatabaseManager::DataType::TEXT},    //user_passhash
            {4, DatabaseManager::DataType::TEXT},    //user_legalname
            {5, DatabaseManager::DataType::TEXT},    //user_phonenumber
            {6, DatabaseManager::DataType::TEXT},    //user_emailaddress
            {7, DatabaseManager::DataType::TEXT},    //user_description
            {8, DatabaseManager::DataType::INTEGER}, //user_permission
            {9, DatabaseManager::DataType::INTEGER}, //user_visibility
            {10, DatabaseManager::DataType::INTEGER} //user_timestamp

        };

        db_manager->prepareStatement(parameter_insert, parameter_types);

        //required bindings already validated
        bindJsonRequiredString(1, "user_name", json_data);
        bindJsonRequiredString(2, "user_salt", json_data);
        bindJsonRequiredString(3, "user_passhash", json_data);

        bindJsonOptionalString(4, "user_legalname", json_data, std::nullopt);
        bindJsonOptionalString(5, "user_phonenumber", json_data, std::nullopt);
        bindJsonOptionalString(6, "user_emailaddress", json_data, std::nullopt);
        bindJsonOptionalString(7, "user_description", json_data, std::nullopt);

        bindJsonOptionalInt(8, "user_permission", json_data, 1);
        bindJsonOptionalInt(9, "user_visibility", json_data, 1);
        bindJsonOptionalInt(10, "user_timestamp", json_data, std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());

        if (!db_manager->executePrepared())
        {
            std::cerr << "Error in insertRecord" << std::endl;
            return false;
        }

        return true;
    }

    /*
    //R in CRUD, Retrieve record by ID
    bool retrieveRecordById(int id) override
    {
        nlohmann::json json_result;
        std::map<int, DatabaseManager::DataType> parameter_types =
        {
            {1, DatabaseManager::DataType::INTEGER}
        };
        std::string sql = "SELECT * FROM Users WHERE user_id = ?;";
        db_manager->prepareStatement(sql, parameter_types);
        db_manager->bindInt(1, id);
        auto prepared_statement = db_manager->getPreparedStatement();

        if (sqlite3_step(prepared_statement) == SQLITE_ROW) {
            // Assuming column indices are in order as per your table schema
            json_result["user_id"] = sqlite3_column_int(prepared_statement, 0);
            json_result["user_name"] = reinterpret_cast<const char*>(sqlite3_column_text(prepared_statement, 1));
            json_result["user_salt"] = reinterpret_cast<const char*>(sqlite3_column_text(prepared_statement, 2));
            json_result["user_passhash"] = reinterpret_cast<const char*>(sqlite3_column_text(prepared_statement, 3));
            json_result["user_legalname"] = reinterpret_cast<const char*>(sqlite3_column_text(prepared_statement, 4));
            json_result["user_phonenumber"] = reinterpret_cast<const char*>(sqlite3_column_text(prepared_statement, 5));
            json_result["user_emailaddress"] = reinterpret_cast<const char*>(sqlite3_column_text(prepared_statement, 6));
            json_result["user_description"] = reinterpret_cast<const char*>(sqlite3_column_text(prepared_statement, 7));
            json_result["user_permission"] = sqlite3_column_int(prepared_statement, 8);
            json_result["user_visibility"] = sqlite3_column_int(prepared_statement, 9);
            json_result["user_timestamp"] = reinterpret_cast<const char*>(sqlite3_column_text(prepared_statement, 10));

            return true;
        }

    }
    */

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
        std::map<int, DatabaseManager::DataType> parameter_indices = 
        {
            {1, DatabaseManager::DataType::INTEGER},
            {2, DatabaseManager::DataType::INTEGER}
        };
        GenericDAO::db_manager->prepareStatement(sql, parameter_indices);
        GenericDAO::db_manager->bindInt(1, 0);
        GenericDAO::db_manager->bindInt(2, id);
        if(!GenericDAO::db_manager->executePrepared())
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
