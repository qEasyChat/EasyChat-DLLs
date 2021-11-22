#pragma once

#include <map>

#include "SimpleIni.h"

#include "DB_Driver.h"
#include "User.h"

class SimpleIni_Manager : public DB_Driver
{
public:
	SimpleIni_Manager();
	SimpleIni_Manager(std::string file_name);

	void open_ini_file(std::string file_name);

	bool check_authentification(std::string username, std::string password_hash) override;
	void add_user(std::string username, std::string password_hash) override;
	void delete_user(std::string username) override;
	void modify_user(std::string username, std::string password_hash) override;

	void add_authentification_entry(std::string username, std::string status, std::string ip) override;
	
private:
	const size_t HASH_SIZE = 128;
	const size_t MAXIMUM_LOG_SIZE = 8 * 1024 * 1024;
	size_t last_id = 0;
	std::map<std::string, User> authentification_map;
	std::string ini_file_name;
	CSimpleIniA ini_file;

	void init_authentification_map();
	bool isUsernameInAuthentificationMap(std::string username);
};

