
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

            db_manager->getParameter<std::string>(1, json_result, "user_name", prepared_statement);
            db_manager->getParameter<std::string>(2, json_result, "user_salt", prepared_statement);
            db_manager->getParameter<std::string>(3, json_result, "user_passhash", prepared_statement);
            db_manager->getParameter<std::string>(4, json_result, "user_legalname", prepared_statement);
            db_manager->getParameter<std::string>(5, json_result, "user_phonenumber", prepared_statement);
            db_manager->getParameter<std::string>(6, json_result, "user_emailaddress", prepared_statement);
            db_manager->getParameter<std::string>(7, json_result, "user_description", prepared_statement);

            db_manager->getParameter<int>(8, json_result, "user_permission", prepared_statement);
            db_manager->getParameter<int>(9, json_result, "user_visibility", prepared_statement);
            db_manager->getParameter<int>(10, json_result, "user_timestamp", prepared_statement);
        }

        return json_result;
    }


    //U in CRUD, Update allowed paramters by ID, prevent update of intrinsically locked fields
    bool updateRecordById(int id, nlohmann::json& json_data) {
        const std::map<std::string, DataType> mutable_fields =
        {
            {"user_name", DataType::TEXT},
            {"user_salt", DataType::TEXT},
            {"user_passhash", DataType::TEXT},
            {"user_legalname", DataType::TEXT},
            {"user_phonenumber", DataType::TEXT},
            {"user_emailaddress", DataType::TEXT},
            {"user_description", DataType::TEXT},
            {"user_permission", DataType::INTEGER},
            {"user_visibility", DataType::INTEGER}
        };
        std::string sql = "UPDATE Users SET ";
        bool first = true;
        int param_index = 1;

        // First Pass: Build SQL Query
        for (const auto& field : mutable_fields) {
            if (json_data.contains(field.first)) {
                if (!first) {
                    sql += ", ";
                }
                sql += field.first + " = ?";
                first = false;
                ++param_index;
            }
        }

        if (first) {
            std::cerr << "No valid fields provided for update." << std::endl;
            return false;
        }

        sql += " WHERE user_id = ?;";
        db_manager->prepareStatement(sql);

        // Second Pass: Bind Parameters
        param_index = 1;
        for (const auto& field : mutable_fields) {
            if (json_data.contains(field.first)) {
                switch (field.second) {
                case DataType::TEXT:
                    db_manager->bindParameter<std::string>(param_index, json_data.at(field.first).get<std::string>());
                    break;
                case DataType::INTEGER:
                    db_manager->bindParameter<int>(param_index, json_data.at(field.first).get<int>());
                    break;
                case DataType::REAL:
                    db_manager->bindParameter<double>(param_index, json_data.at(field.first).get<double>());
                    break;
                    // Add cases for other data types as needed
                }
                ++param_index;
            }
        }

        // Binding the user ID
        db_manager->bindParameter<int>(param_index, id);

        // Execute the prepared statement
        if (!db_manager->executePrepared()) {
            std::cerr << "Error in updateRecordById" << std::endl;
            return false;
        }

        return true;
    }

    //D in CRUD
    bool deleteRecordById(int id) override {
        std::string sql = "UPDATE Users SET user_visibility = ? WHERE user_id = ?;";
        db_manager->prepareStatement(sql);
        db_manager->bindParameter<int>(1, 0);
        db_manager->bindParameter<int>(2, id);
        if (!db_manager->executePrepared())
        {
            std::cerr << "Error in deleteRecordById." << std::endl;
            return false;
        }
        std::cout << "Users table is append only. User has been set to invisible." << std::endl;
        return true;
    }


    bool existenceOfRecordByField(const std::string& field_name, const std::string& value) {
        return GenericDAO::existenceOfRecordByField("Users", field_name, value);
    }

    std::optional<int> getIdGivenUsername(const std::string& username) {
        std::string sql = "SELECT user_id FROM Users WHERE user_name = ?;";

        db_manager->prepareStatement(sql);
        db_manager->bindParameter<std::string>(1, username);

        if (sqlite3_step(db_manager->getPreparedStatement()) == SQLITE_ROW) {
            nlohmann::json result_json;
            db_manager->getParameter<int>(0, result_json, "user_id", db_manager->getPreparedStatement());
            sqlite3_finalize(db_manager->getPreparedStatement());

            if (result_json.contains("user_id")) {
                return result_json["user_id"].get<int>();
            }
        }

        sqlite3_finalize(db_manager->getPreparedStatement());
        return std::nullopt;
    }

    
};

#endif //USERDAO_HPP