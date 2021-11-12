#pragma once
#include <string>

class DB_Driver
{
public:
	virtual bool check_authentification(std::string username, std::string password_hash) = 0;
	virtual void add_user(std::string username, std::string password_hash) = 0;
	virtual void delete_user(std::string username) = 0;
	virtual void modify_user(std::string username, std::string password_hash) = 0;

	virtual void add_authentification_entry(std::string username, std::string status, std::string ip) = 0;
};

