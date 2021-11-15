#include "pch.h"

#include "Database_Manager.h"

#include "SimpleIni_Manager.h"
#include "SQLite_Manager.h"

std::shared_ptr<DB_Driver> Database_Manager::get_db_driver(Database_Driver_Type database_driver_type, std::string ini_file_path)
{
	std::shared_ptr<DB_Driver> driver;
	switch (database_driver_type)
	{
		case Database_Driver_Type::INI:
			{
				driver = std::shared_ptr<DB_Driver>(new SimpleIni_Manager(ini_file_path));
				break;
			}
		case Database_Driver_Type::SQLITE:
			{
				driver = std::shared_ptr<DB_Driver>(new SQLite_Manager(ini_file_path));
				break;
			}
		default:
			{
				driver = nullptr;
			}

	}
	return driver;
}
