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