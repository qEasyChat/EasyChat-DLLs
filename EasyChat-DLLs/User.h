#pragma once
#include <string>

class User
{
public:
	User();
	User(const size_t id, const std::string& username, const std::string& password_hash);

	~User();
	size_t get_id() const;
	void set_id(const size_t id);
	std::string get_username() const;
	void set_username(const std::string& username);
	std::string get_password_hash() const;
	void set_password_hash(const std::string& password_hash);
private:
	size_t id;
private:
	std::string username, password_hash;
};

