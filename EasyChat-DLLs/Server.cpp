#include "pch.h"

#define SERVER_EXPORTS
#include "Server.h"

#include <iostream>

#include "Crypto_Manager.h"
#include "Utils.h"

Server::Server(std::string name, size_t port) {
	this->name = name;
	
	this->server_connection = std::unique_ptr<Connection>(new Connection(port, LOOPBACK_ADDR, "SERVER"));
	struct addrinfo hints;
	struct addrinfo* server = NULL;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	const char OPTION_VALUE = 1;

	getaddrinfo(LOOPBACK_ADDR, std::to_string(port).c_str(), &hints, &server);
	server_sock = socket(server->ai_family, server->ai_socktype, server->ai_protocol);
	setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &OPTION_VALUE, sizeof(int)); //Make it possible to re-bind to a port that was used within the last 2 minutes
	setsockopt(server_sock, IPPROTO_TCP, TCP_NODELAY, &OPTION_VALUE, sizeof(int)); //Used for interactive programs


	memset(server_addr.sin_zero, '\0', sizeof(server_addr.sin_zero));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port_number);

	InetPton(AF_INET, (PCWSTR)LOOPBACK_ADDR, &server_addr.sin_addr.s_addr);

	client_addr_size = sizeof(client_addr);

	if (bind(server_sock, server->ai_addr, (int)server->ai_addrlen) != 0) {
		std::cerr << "binding unsuccessful" << std::endl << WSAGetLastError() <<std::endl;
		exit(1);
	}

	if (listen(server_sock, SOMAXCONN) != 0) {
		std::cerr << "listening unsuccessful" << std::endl;
		exit(1);
	}
}

void Server::start() {
	std::thread server_module_worker(&Server::server_module, this);
	std::thread server_command_manager_worker(&Server::server_command_manager, this);
	server_module_worker.detach();
	server_command_manager_worker.detach();
	while (true)
	{
	}
}

void Server::server_module()
{
	while (1) {
		SOCKET client_sock;
		client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_addr_size);

		mtx.lock();
		char ip_c[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, (struct sockaddr*)&client_addr, ip_c, INET_ADDRSTRLEN);
		std::string ip = std::string(ip_c);
		std::cout << "connection successful " << ip << std::endl;
		std::shared_ptr<Connection> client_connection = std::shared_ptr<Connection>(new Connection());
		if (client_connection == nullptr)
		{
			Utils::memory_error();
		}
		client_connection->set_socket(client_sock);
		std::string username = client_connection->recive_message();
		std::string password_hash = client_connection->recive_message();
		client_connection->set_username(username);

		client_connection->send_message(this->name);

		std::string login_message;
		std::string status;
		if (this->db_driver->check_authentification(username, password_hash))
		{
			login_message = "OK";
			status = "SUCCESS";
			std::cout << "authentification successful for " << username << std::endl;
		}
		else
		{
			login_message = "RETRY";
			status = "FAILURE";
			std::cout << "authentification failure for " << username << std::endl;
		}
		this->db_driver->add_authentification_entry(username, status, ip);
		client_connection->send_message(login_message);
		if (login_message == "OK") {
			this->username_connection_map[username] = client_connection;
			std::thread worker(&Server::reciver, this, client_connection);
			workers[client_connection] = std::move(worker);
			this->send_online_users(client_connection);
		}
		client_connection->send_message("Welcome to " + this->name);
		this->notify_users_new_connection(username);
		mtx.unlock();
	}

	for (auto const& item : this->workers) {
		if (workers[item.first].joinable()) {
			workers[item.first].join();
		}
	}
}


void Server::reciver(std::shared_ptr<Connection> client_connection) {
	std::string username = client_connection->get_username();
	std::cout << "reciver started for " << username << std::endl;
	std::string message;
	try {
		while (true) {
			message = client_connection->recive_message();
			if(message[0] == '/')
			{
				this->run_command(client_connection, message);
			} else
			{
				send_to_all(username, message);
			}
		}
	}
	catch (Client_Down_Exception exception) {
		std::cout << username << " has disconnected" << std::endl;
		this->remove_user(username);
	}
}

void Server::remove_user(std::string username)
{
	mtx.lock();
	this->username_connection_map.erase(username);
	this->notify_users_user_disconnection(username);
	mtx.unlock();

}


std::string Server::get_wrapped_message(std::string username, std::string message)
{
	std::string wrapped_message = username + ": " + message;
	return wrapped_message;
}

void Server::send_to_all(std::string username, std::string message) {
	mtx.lock();
	std::string pkg = "";
	std::string wrapped_message = this->get_wrapped_message(username, message);
	for (auto const& item : this->username_connection_map) {
		if(item.first != username)
		{
			item.second->send_message(wrapped_message);
		}
	}
	mtx.unlock();
	std::this_thread::sleep_for(WAIT_PERIOD);
}

void Server::notify_users_new_connection(std::string username)
{
	std::string message = username + " has connected";
	for (auto const& item : this->username_connection_map) {
		if (item.first != username)
		{
			item.second->send_message(message);
		}
	}
	std::this_thread::sleep_for(WAIT_PERIOD);
}

void Server::notify_users_user_disconnection(std::string username)
{
	std::string message = username + " has disconnected";
	for (auto const& item : this->username_connection_map) {
		if (item.first != username)
		{
			item.second->send_message(message);
		}
	}
	std::this_thread::sleep_for(WAIT_PERIOD);
}

void Server::run_command(std::shared_ptr<Connection> user_connection, std::string command)
{
	if (command == "/online")
	{
		send_online_users(user_connection);
	}
}

void Server::send_online_users(std::shared_ptr<Connection> user_connection)
{
	std::string online_users_message = "ONLINE USERS\n";
	std::string username = user_connection->get_username();
	for(auto const& item : this->username_connection_map)
	{
		online_users_message += "* " + item.first;
		if(item.first == username)
		{
			online_users_message += " (you)";
		}
		online_users_message += "\n";
	}
	user_connection->send_message(online_users_message);
}

void Server::connect_to_database(Database_Driver_Type driver_type, std::string ini_file_path)
{
	this->db_driver = Database_Manager::get_db_driver(driver_type, ini_file_path);
}

void Server::add_new_user(std::string username, std::string password)
{
	std::string password_hash = crypto_manager.get_sha3_512_hash(password);
	this->db_driver->add_user(username, password_hash);
}

void Server::delete_user(std::string username)
{
	this->db_driver->delete_user(username);
}

void Server::modify_user(std::string username, std::string password_hash)
{
	this->db_driver->modify_user(username, password_hash);
}


void Server::server_command_manager()
{
	while (1)
	{
		std::string command_line;
		std::getline(std::cin, command_line);
		std::istringstream command_stream(command_line);
		std::string command;
		command_stream >> command;
		if (command.find("/add") != std::string::npos)
		{
			std::string username, password_hash;
			command_stream >> username >> password_hash;
			this->add_new_user(username, password_hash);
			std::cout << "added user " << username << std::endl;
		} else if (command.find("/delete") != std::string::npos)
		{
			std::string username;
			command_stream >> username;
			this->delete_user(username);
			std::cout << "deleted user " << username << std::endl;
		} else if (command.find("/modify") != std::string::npos)
		{
			std::string username, password;
			command_stream >> username >> password;
			std::string password_hash = crypto_manager.get_sha3_512_hash(password);
			this->modify_user(username, password_hash);
			std::cout << "modified user's " << username << " password" << std::endl;
		}

	}
}