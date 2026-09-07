#pragma once

#include "IBookingRepository.hpp"
#include "../http/dto/CreateBookingDto.hpp"
#include "../models/NewBooking.hpp"
#include "CreateBookingResult.hpp"

#include <vector>
#include <chrono>
#include <optional>
#include <string>

class BookingService
{
public:
	explicit BookingService(IBookingRepository& repository);

	CreateBookingResult create_booking(
		const CreateBookingDto& dto);

	std::vector<Booking> get_all_bookings();
	
	bool delete_booking(std::int64_t id);

private:
	std::optional<NewBooking> prepare_booking(
		const CreateBookingDto& dto) const;

	std::optional<std::chrono::sys_seconds>
		parse_utc_time(const std::string& text) const;

	bool has_utc_time_shape(const std::string& text) const;

	IBookingRepository& repository_;
};