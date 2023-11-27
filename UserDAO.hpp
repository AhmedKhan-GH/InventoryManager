
#ifndef USERDAO_HPP
#define USERDAO_HPP

#include "DatabaseManager.hpp"
#include "GenericDAO.hpp"
#include <string>
#include <stdexcept>
#include "nlohmann\\json.hpp"

class UserDAO : public GenericDAO {
public:
    UserDAO(DatabaseManager* db_manager) : db_manager(db_manager) {}

    bool validateJsonFields(const nlohmann::json& json_data) {
        const std::vector<std::pair<std::string, nlohmann::json::value_t>> requiredFields = {
            {"user_name", nlohmann::json::value_t::string},
            {"user_salt", nlohmann::json::value_t::string},
            {"user_passhash", nlohmann::json::value_t::string},
        };

        for (const auto& field : requiredFields) {
            if (!json_data.contains(field.first) || json_data[field.first].type() != field.second) {
                std::cerr << "Error: Missing or incorrect JSON field: " << field.first << std::endl;
                return false;
            }
        }

        return true;
    }

    bool insertRecord(const nlohmann::json& json_data)
    {
        std::string insert_sql = "INSERT INTO Users (user_name, user_salt, user_passhash, "
            "user_legalname, user_phonenumber, user_emailaddress, user_description) "
            "VALUES (?, ?, ?, ?, ?, ?, ?);";

        std::map<int, DataType> insert_parameters =
        {
            {1, DataType::TEXT},
            {2, DataType::TEXT},
            {3, DataType::TEXT},
            {4, DataType::TEXT},
            {5, DataType::TEXT},
            {6, DataType::TEXT},
            {7, DataType::TEXT}
        };

        std::string user_name = json_data["user_name"];
        std::string user_salt = json_data["user_salt"];
        std::string user_passhash = json_data["user_passhash"];
        std::string user_legalname = json_data.value("user_legalname", "");
        std::string user_phonenumber = json_data.value("user_phonenumber", "");
        std::string user_emailaddress = json_data.value("user_emailaddress", "");
        std::string user_description = json_data.value("user_description", "");

        db_manager->prepareStatement(insert_sql, insert_parameters);

        db_manager->bindString  (1, user_name);           // user_name is a string
        db_manager->bindString  (2, user_salt);           // user_salt is a string
        db_manager->bindString  (3, user_passhash);       // user_passhash is a string
        db_manager->bindString  (4, user_legalname);      // user_legalname is a string
        db_manager->bindString  (5, user_phonenumber);    // user_phonenumber is a string
        db_manager->bindString  (6, user_emailaddress);   // user_emailaddress is a string
        db_manager->bindString  (7, user_description);   // user_description is a string


        if (!db_manager->executePrepared())
        {
            std::cerr << "Error inserting user." << std::endl;
            return false;
        }

        return true;
    }
    
    nlohmann::json selectRecordById(int id) {
       /*
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
        */
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

    void deleteRecordById(int id) override {
        std::string delete_sql = "DELETE FROM Users WHERE user_id = ?;";
        std::map<int, DataType> delete_parameter = { { 1, DataType::INTEGER } };

        db_manager->prepareStatement(delete_sql, delete_parameter);
        db_manager->bindInt(1, id);
        if (!db_manager->executePrepared())
        {
            std::cerr << "Error deleting user." << std::endl;
        }
    }

private:
    DatabaseManager* db_manager;
};

#endif // USERDAO_HPP
