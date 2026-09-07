#include "BookingService.hpp"
#include <sstream>


BookingService::BookingService(IBookingRepository& repository): repository_(repository){}

CreateBookingResult BookingService::create_booking(
	const CreateBookingDto& dto) {
	auto prepared = prepare_booking(dto);
	CreateBookingResult booking_result;
	if (!prepared) {
		booking_result.status = CreateBookingStatus::invalid_data;
		booking_result.booking = std::nullopt;
		return booking_result;
	}

	booking_result.booking = repository_.try_create(prepared.value());
	if (booking_result.booking) {
		booking_result.status = CreateBookingStatus::created;
	}
	else {
		booking_result.status = CreateBookingStatus::conflict;
	}
	return booking_result;
}

std::vector<Booking> BookingService::get_all_bookings() {
	return repository_.get_all();
}

bool BookingService::delete_booking(std::int64_t id) {
	return repository_.remove(id);
}

std::optional<NewBooking> BookingService::prepare_booking(
	const CreateBookingDto& dto) const {
	if (dto.resource_id <= 0) return std::nullopt;
	//parse start_time;
	auto start = parse_utc_time(dto.start_time);
	if (!start) {
		return std::nullopt;
	}
	auto end = parse_utc_time(dto.end_time);
	if (!end) {
		return std::nullopt;
	}
	if (start.value() < end.value()) {
		return NewBooking { dto.resource_id, start.value(), end.value() };
	}
	else {
		return std::nullopt;
	}
}

std::optional<std::chrono::sys_seconds>
BookingService::parse_utc_time(const std::string& text) const {
	if (!has_utc_time_shape(text)) return std::nullopt;
	std::istringstream stream(text);
	std::chrono::sys_seconds time;
	std::chrono::from_stream(
		stream,
		"%Y-%m-%dT%H:%M:%SZ",
		time
	);
	if (stream.fail())
	{
		return std::nullopt;
	}
	return time;
}

bool BookingService::has_utc_time_shape(const std::string& text) const {
	if (text.size() != 20) return false;
	int i = 0;
	for (; i < 20; i++) {
		switch (i) {
		case 4: {
			if (text[4] != '-') return false;
			break;
		}
		case 7: {
			if (text[7] != '-') return false;
			break;
		}
		case 10: {
			if (text[10] != 'T') return false;
			break;
		}
		case 13: {
			if (text[13] != ':') return false;
			break;
		}
		case 16: {
			if (text[16] != ':') return false;
			break;
		}
		case 19: {
			if (text[19] != 'Z') return false;
			break;
		}
		default: {
			if (text[i] < '0' || text[i] > '9') return false;
			break;
		}
		}
	}
	return true;
}