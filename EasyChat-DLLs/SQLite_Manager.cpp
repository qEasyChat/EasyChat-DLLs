#include "pch.h"

#include "SQLite_Manager.h"

#include <iostream>

#include "Utils.h"

SQLite_Manager::SQLite_Manager(std::string database_file_path)
{
    this->database_file_path = database_file_path;
    this->database = std::shared_ptr<sqlite3>(nullptr);
    auto database_pointer = this->database.get();
    size_t return_code = sqlite3_open(database_file_path.c_str(), &database_pointer);
    this->database.reset(database_pointer);

    if(return_code)
    {
        std::cerr << "can't open SQLite database" << std::endl;
        exit(1);
    } else
    {
        std::cout << "SQLite database opened successfully" << std::endl;
    }
    this->create_user_table();
    this->create_authentification_table();
}

int SQLite_Manager::callback(void* data, int entries_number, char** entries, char** col_names) {
    Table* table = static_cast<Table*>(data);
    try {
        table->emplace_back(entries, entries + entries_number);
    }
    catch (...) {
        return 1;
    }
}

void SQLite_Manager::create_user_table()
{
    Table return_table;
    std::cout << "creating user table if it dosen't exists" << std::endl;
    std::string query = Utils::get_query("SQL/create-user-table.sql");
	char* zErrMsg = 0;
    sqlite3_exec(this->database.get(), query.c_str(), this->callback, &return_table, &zErrMsg);
}


void SQLite_Manager::add_user(std::string username, std::string password_hash)
{
    Table return_table;
    std::string query = Utils::get_query("SQL/insert-new-user.sql");
    query = Utils::replace(query, "USERNAME", username);
    query = Utils::replace(query, "PASSWORD", password_hash);
    char* zErrMsg = 0;
    sqlite3_exec(this->database.get(), "BEGIN TRANSACTION;", NULL, NULL, NULL);
    sqlite3_exec(this->database.get(), query.c_str(), this->callback, &return_table, &zErrMsg);
    sqlite3_exec(this->database.get(), "COMMIT;", NULL, NULL, NULL);
}

bool SQLite_Manager::check_authentification(std::string username, std::string password_hash)
{
    Table return_table;
    std::string query = Utils::get_query("SQL/check-authentification.sql");
    query = Utils::replace(query, "USERNAME", username);
    query = Utils::replace(query, "PASSWORD", password_hash);
    char* zErrMsg = 0;
    sqlite3_exec(this->database.get(), query.c_str(), this->callback, &return_table, &zErrMsg);
    if(return_table.size() == 1)
    {
        return true;
    }
    return false;
}

void SQLite_Manager::create_authentification_table()
{
    Table return_table;
    std::cout << "creating authentification table if it dosen't exists" << std::endl;
    std::string query = Utils::get_query("SQL/create-authentification-table.sql");
    char* zErrMsg = 0;
    sqlite3_exec(this->database.get(), query.c_str(), this->callback, &return_table, &zErrMsg);
}

std::shared_ptr<User> SQLite_Manager::get_user(std::string username)
{
    Table return_table;
    std::string query = Utils::get_query("SQL/get-user.sql");
    query = Utils::replace(query, "USERNAME", username);
    char* zErrMsg = 0;
    sqlite3_exec(this->database.get(), query.c_str(), this->callback, &return_table, &zErrMsg);
    if (return_table.size() == 1)
    {
        std::vector<std::string> row = return_table.at(0);
        size_t id = std::stoi(row.at(0));
        std::string username = row.at(1);
        std::string password_hash = row.at(2);

        std::shared_ptr<User> user = std::shared_ptr<User>(new User(id, username, password_hash));
        return user;
    }
    return nullptr;
}

void SQLite_Manager::add_authentification_entry(std::string username, std::string status, std::string ip)
{
    time_t now = time(0);
    std::string timestamp = ctime(&now);
    Table return_table;
    std::string query = Utils::get_query("SQL/insert-authentification-entry.sql");
    std::shared_ptr<User> user = this->get_user(username);
    query = Utils::replace(query, "USER_ID", std::to_string(user->get_id()));
    query = Utils::replace(query, "STATUS", status);
    query = Utils::replace(query, "IP", ip);
    query = Utils::replace(query, "TIMESTAMP", timestamp);
    char* zErrMsg = 0;
    sqlite3_exec(this->database.get(), "BEGIN TRANSACTION;", NULL, NULL, NULL);
    sqlite3_exec(this->database.get(), query.c_str(), this->callback, &return_table, &zErrMsg);
    sqlite3_exec(this->database.get(), "COMMIT;", NULL, NULL, NULL);
}

void SQLite_Manager::delete_user(std::string username)
{
    Table return_table;
    std::string query = Utils::get_query("SQL/delete-user.sql");
    query = Utils::replace(query, "USERNAME", username);
    char* zErrMsg = 0;
    sqlite3_exec(this->database.get(), "BEGIN TRANSACTION;", NULL, NULL, NULL);
    sqlite3_exec(this->database.get(), query.c_str(), this->callback, &return_table, &zErrMsg);
    sqlite3_exec(this->database.get(), "COMMIT;", NULL, NULL, NULL);
}

void SQLite_Manager::modify_user(std::string username, std::string password_hash)
{
    Table return_table;
    std::string query = Utils::get_query("SQL/modify-user.sql");
    query = Utils::replace(query, "USERNAME", username);
    query = Utils::replace(query, "PASSWORD", password_hash);
    char* zErrMsg = 0;
    sqlite3_exec(this->database.get(), "BEGIN TRANSACTION;", NULL, NULL, NULL);
    sqlite3_exec(this->database.get(), query.c_str(), this->callback, &return_table, &zErrMsg);
    sqlite3_exec(this->database.get(), "COMMIT;", NULL, NULL, NULL);
}

