#pragma once
#include "DB_Driver.h"

#include <memory>
#include <string>
#include <vector>

#include <sqlite3.h>

#include "User.h"

class SQLite_Manager : public DB_Driver
{
public:
	SQLite_Manager(std::string database_file_path);
	void add_user(std::string username, std::string password_hash) override;
	void delete_user(std::string username) override;
	void modify_user(std::string username, std::string password_hash) override;
	bool check_authentification(std::string username, std::string password_hash) override;
	void add_authentification_entry(std::string username, std::string status, std::string ip) override;
private:
	typedef std::vector<std::vector<std::string>> Table;
	std::string database_file_path;
	std::shared_ptr<sqlite3> database;

	static int callback(void* data, int entries_number, char** entries, char** col_names);
	void create_user_table();
	void create_authentification_table();

	std::shared_ptr<User> get_user(std::string username);
};
