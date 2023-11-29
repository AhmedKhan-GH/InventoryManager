#ifndef LOGINDAO_HPP
#define LOGINDAO_HPP

#include "GenericDAO.hpp"

class LoginDAO : public GenericDAO
{
public:

    //update individual column

    //C
    // Insert a new record into the database
    virtual bool insertRecord(const nlohmann::json& json_data) override
    {
        //sql query stays as normal
        std::string parameter_insert =
            "INSERT INTO Logins (login_user, login_success, login_timestamp) VALUES (?, ?, ?);";

        db_manager->prepareStatement(parameter_insert);

        //required bindings already validated
        db_manager->bindParameter<std::string>(1, json_data["login_user"]);
        db_manager->bindParameter<int>(2, json_data["login_success"]);
        db_manager->bindParameter<int>(3, std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());

        if (!db_manager->executePrepared())
        {
            std::cerr << "Error in insertRecord" << std::endl;
            return false;
        }

        return true;
    }

    //R
    virtual nlohmann::json retrieveRecordById(int id) override
    {
        nlohmann::json json_result;

        std::string sql = "SELECT * FROM Logins WHERE login_id = ?;";
        db_manager->prepareStatement(sql);
        db_manager->bindParameter<int>(1, id);
        sqlite3_stmt* prepared_statement = db_manager->getPreparedStatement();

        if (sqlite3_step(prepared_statement) == SQLITE_ROW) {
            // Assuming column indices are in order as per your table schema

            db_manager->getParameter<std::string>(1, json_result, "login_user", prepared_statement);
            db_manager->getParameter<int>(2, json_result, "login_success", prepared_statement);
            db_manager->getParameter<int>(3, json_result, "login_timestamp", prepared_statement);

            return json_result;
        }
    }

    //U
    bool updateRecordById(int id, nlohmann::json& json_data) override
    {
        std::cout << "Logins are append only and cannot be updated." << std::endl;
    }

    //D
    // Delete a record by its ID
    bool deleteRecordById(int id)
    {
        std::cout << "Logins are append only and cannot be deleted." << std::endl;
        return false;
    }

    virtual bool existenceOfRecordByField(const std::string& table_name, const std::string& field_name, const std::string& value)
    {
        GenericDAO::existenceOfRecordByField("Logins", field_name, value);
    }
private:

};

#endif //LOGINDAO_HPP