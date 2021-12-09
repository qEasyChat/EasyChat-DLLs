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


size_t Bridge::get_driver_type(std::string db_type)
{

	if (db_type == "INI")
	{
		return 0;
	}
	if (db_type=="SQLite")
	{
		return 1;
	}
	return -1;
}
