#pragma once

#include <pqxx/pqxx>
#include <string>

class Database
{
public:
	explicit Database(const std::string& connection_string);

	pqxx::connection create_connection() const;

private:
	std::string connection_string_;
};