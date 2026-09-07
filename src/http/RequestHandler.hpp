#pragma once
#include "HttpTypes.hpp"
#include "../booking/BookingService.hpp"
#include <optional>
#include <cstdint>

class RequestHandler {
public:
	explicit RequestHandler(BookingService& booking_service);

	Response handle_ping(const Request& request) const;

	Response handle_not_found(const Request& request) const;

	Response handle_not_allowed(const Request& request) const;

	Response handle_book(const Request& request) const;

	Response handle_get_bookings(const Request& request) const;

	Response handle_delete_bookings(const Request& request) const;
private:
	Response handle_bad_request(const Request& request) const;

	Response handle_conflict(const Request& request) const;

	std::optional<std::int64_t> parse_booking_id(beast::string_view text)const;

	BookingService& booking_service_;
};