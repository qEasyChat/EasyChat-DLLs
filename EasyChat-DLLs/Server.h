#pragma once
#pragma comment( lib, "ws2_32.lib")
#include <functional>
#include <winsock2.h>
#include <Ws2tcpip.h>

#include <thread>
#include <mutex>
#include <vector>
#include <map>
#include <sstream>
#include <unordered_map>

#include "Connection.h"
#include "Crypto_Manager.h"
#include "Database_Manager.h"


#ifdef SERVER_EXPORTS
#define SERVER_API __declspec(dllexport)
#else
#define SERVER_API __declspec(dllimport)
#endif

typedef std::function<void(std::shared_ptr<Connection>)> FunctionType;

#define LOOPBACK_ADDR	"192.168.56.1"
#define DEFAULT_PORT 50005
#define WAIT_PERIOD std::chrono::milliseconds(500)

using pointer_function = void (*)(std::shared_ptr<Connection>);

class SERVER_API Server {
public:
	Server();
	Server(std::string name, size_t port = DEFAULT_PORT);

	void start();

	void connect_to_database(Database_Driver_Type driver_type, std::string ini_file_path);
	
private:
	std::string name;

	std::thread send_worker;

	Crypto_Manager crypto_manager;
	std::mutex mtx;
	std::map<std::string, std::shared_ptr<Connection>> username_connection_map;
	std::shared_ptr<Connection> server_connection;
	std::map<std::shared_ptr<Connection> , std::thread> workers;
	std::map<std::string, pointer_function> commands;

	struct sockaddr_in server_addr, client_addr;
	SOCKET server_sock;
	socklen_t client_addr_size;
	int port_number;

	std::shared_ptr<DB_Driver> db_driver;

	void send_to_all(std::string username, std::string message);
	void reciver(std::shared_ptr<Connection> client_connection);
	void server_module();

	std::string get_wrapped_message(std::string username, std::string message);

	void notify_users_new_connection(std::string username);
	void notify_users_user_disconnection(std::string username);

	void remove_user(std::string username);

	void build_commands_map();
	void run_command(std::shared_ptr<Connection> user_connection, std::string command);
	void send_online_users(std::shared_ptr<Connection> user_connection);

	void server_command_manager();
	void add_new_user(std::string username, std::string password);
	void delete_user(std::string username);
	void modify_user(std::string username, std::string password_hash);
};