#pragma once
#include <string>
#include <cstddef>

struct Config
{

	std::string db_connection_string;
	unsigned short port;
	std::size_t worker_count;
	std::size_t pool_size;
};

Config load_config();

std::size_t parse_positive_number(
	const char* text,
	const std::string& variable_name
);