#include "pch.h"
#include "Bridge.h"

Server* Bridge::new_server(const wchar_t* server_name, size_t port)
{
	std::wstring server_name_ws(server_name);
	std::string server_name_str(server_name_ws.begin(), server_name_ws.end());
	return new Server(server_name_str, port);
}

void Bridge::delete_server(Server* server)
{
	if (server != nullptr)
	{
		delete server;
		server = nullptr;
	}
}

size_t Bridge::get_driver_type(const wchar_t* db_type)
{
	std::wstring db_type_ws(db_type);
	std::string db_type_str(db_type_ws.begin(), db_type_ws.end());

	if (db_type_str == "INI")
	{
		return 0;
	}
	if (db_type_str =="SQLite")
	{
		return 1;
	}
	return -1;
}

void Bridge::start_server(Server* server)
{
	server->start();
}

void Bridge::connect_server_to_database(Server* server, const wchar_t* db_type, const wchar_t* file_path)
{
	std::wstring db_type_ws(db_type);
	std::string db_type_str(db_type_ws.begin(), db_type_ws.end());
	std::wstring file_path_ws(file_path);
	std::string file_path_str(file_path_ws.begin(), file_path_ws.end());
	Database_Driver_Type db_driver_type;
	if (db_type_str == "INI")
	{
		db_driver_type = Database_Driver_Type::INI;
	}
	if (db_type_str == "SQLite")
	{
		db_driver_type = Database_Driver_Type::SQLITE;
	}
	server->connect_to_database(db_driver_type, file_path_str);
}
