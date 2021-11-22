#pragma once
#include <string>


#ifdef DB_DRIVER_EXPORTS
#define DB_DRIVER_API __declspec(dllexport)
#else
#define DB_DRIVER_API __declspec(dllimport)
#endif


class DB_DRIVER_API DB_Driver
{
public:
	DB_Driver();
	~DB_Driver();
	virtual bool check_authentification(std::string username, std::string password_hash) = 0;
	virtual void add_user(std::string username, std::string password_hash) = 0;
	virtual void delete_user(std::string username) = 0;
	virtual void modify_user(std::string username, std::string password_hash) = 0;

	virtual void add_authentification_entry(std::string username, std::string status, std::string ip) = 0;
};

