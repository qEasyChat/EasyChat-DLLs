#pragma once

#ifdef BRIDGE_EXPORTS
#define BRIDGE_API __declspec(dllexport)
#else
#define BRIDGE_API __declspec(dllimport)
#endif

#include "Server.h"

namespace Bridge
{
	extern "C" __declspec(dllexport) Server* new_server(const wchar_t* server_name, size_t port);
	extern "C" __declspec(dllexport) void delete_server(Server* server);
	extern "C" __declspec(dllexport) size_t get_driver_type(const wchar_t* db_type);
	extern "C" __declspec(dllexport) void start_server(Server* server);
	extern "C" __declspec(dllexport) void connect_server_to_database(Server* server, const wchar_t* db_type, const wchar_t* file_path);
};
