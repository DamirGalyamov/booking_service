#pragma once

#include "../models/Booking.hpp"

#include <optional>

enum class CreateBookingStatus
{
	created,
	invalid_data,
	conflict
};

struct CreateBookingResult
{
	CreateBookingStatus status;
	std::optional<Booking> booking;
};