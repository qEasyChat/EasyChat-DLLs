#include "pch.h"
#include "Bridge.h"

Server* Bridge::new_server(std::string server_name, size_t port)
{
	return new Server(server_name, port);
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
