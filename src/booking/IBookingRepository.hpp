#pragma once

#include "../models/Booking.hpp"
#include "../models/NewBooking.hpp"

#include <cstdint>
#include <optional>
#include <vector>

class IBookingRepository
{
public:
	virtual ~IBookingRepository() = default;

	virtual std::optional<Booking> try_create(
		const NewBooking& booking) = 0;

	virtual std::vector<Booking> get_all() = 0;

	virtual bool remove(std::int64_t id) = 0;
};