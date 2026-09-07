#pragma once

#include <chrono>
#include <cstdint>

struct Booking {
	std::int64_t id;
	std::int64_t resource_id;

	std::chrono::sys_seconds start_time;
	std::chrono::sys_seconds end_time;

};