#include "Database.hpp"

Database::Database(
	const std::string& connection_string)
	: connection_string_(connection_string)
{
}


pqxx::connection Database::create_connection() const{
	pqxx::connection connection(connection_string_);
	return connection;
}