#pragma once

#ifdef DATABASE_MANAGER_EXPORTS
#define DATABASE_MANAGER_API __declspec(dllexport)
#else
#define DATABASE_MANAGER_API __declspec(dllimport)
#endif

#include <memory>

#include "DB_Driver.h"

enum DATABASE_MANAGER_API Database_Driver_Type
{
	INI,
	SQLITE
};

namespace Database_Manager
{
	std::shared_ptr<DB_Driver> DATABASE_MANAGER_API get_db_driver(Database_Driver_Type database_driver_type, std::string ini_file_path);
}
