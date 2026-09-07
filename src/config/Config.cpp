#include "Config.hpp"
#include <charconv>
#include <stdexcept>
#include <cstdlib>

Config load_config() {
	std::size_t port_s;
	std::size_t worker_count;
	std::size_t pool_size;
	const char* raw = std::getenv(
		"BOOKING_DB_CONNECTION");
	if (raw == nullptr) {
		throw std::runtime_error("BOOKING_DB_CONNECTION is not set");
	}
	std::string db_connection_string{ raw };
	port_s = parse_positive_number(std::getenv(
		"BOOKING_PORT"), "BOOKING_PORT");
	if (port_s > 65535) {
		throw std::runtime_error("BOOKING_PORT must be in range 1..65535");
	}
	worker_count = parse_positive_number(std::getenv(
		"BOOKING_WORKERS"), "BOOKING_WORKERS");
	pool_size = parse_positive_number(std::getenv(
		"BOOKING_POOL_SIZE"), "BOOKING_POOL_SIZE");
	unsigned short port = static_cast<unsigned short>(
		port_s
		);

	return Config{db_connection_string, port, worker_count, pool_size};
}

std::size_t parse_positive_number(
	const char* text,
	const std::string& variable_name
) {
	if (text == nullptr) {
		throw std::runtime_error(variable_name +
			" is not set");
	}
	std::size_t value;
	std::string text_string = text;

	auto result = std::from_chars(text_string.data(), text_string.data() + text_string.size(), value);
	if (result.ec == std::errc{} && result.ptr == text_string.data() + text_string.size()) {
		if (value > 0) {
			return value;
		}
	}
	throw std::runtime_error(variable_name +
		" must be a positive integer");
}