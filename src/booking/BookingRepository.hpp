#pragma once

#include "IBookingRepository.hpp"
#include "../database/ConnectionPool.hpp"
#include "../models/Booking.hpp"
#include "../models/NewBooking.hpp"
#include <vector>
#include <optional>
#include <cstdint>

class BookingRepository: public IBookingRepository {
public:
	explicit BookingRepository(ConnectionPool& pool);

	std::optional<Booking> try_create(const NewBooking& booking) override;

	std::vector<Booking> get_all()override;

	bool remove(std::int64_t id)override;

private:
	ConnectionPool& pool_;
};