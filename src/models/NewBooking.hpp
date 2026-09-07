#pragma once

#include <chrono>
#include <cstdint>

struct NewBooking
{
	std::int64_t resource_id;
	std::chrono::sys_seconds start_time;
	std::chrono::sys_seconds end_time;
};