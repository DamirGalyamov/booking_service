#pragma once

#include <cstdint>
#include <string>

struct CreateBookingDto
{
	std::int64_t resource_id;
	std::string start_time;
	std::string end_time;
};