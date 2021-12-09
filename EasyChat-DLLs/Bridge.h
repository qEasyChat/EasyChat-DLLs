#pragma once

#ifdef BRIDGE_EXPORTS
#define BRIDGE_API __declspec(dllexport)
#else
#define BRIDGE_API __declspec(dllimport)
#endif

#include "Server.h"

namespace Bridge
{
	__declspec(dllexport) Server* new_server(std::string server_name, size_t port);
	__declspec(dllexport) void delete_server(Server* server);

	__declspec(dllexport) size_t get_driver_type(std::string db_type);
};
