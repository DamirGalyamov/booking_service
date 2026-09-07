#include "RequestHandler.hpp"
#include "dto/CreateBookingDto.hpp"
#include "../booking/CreateBookingResult.hpp"

#include <vector>
#include <cstdint>
#include <charconv>

RequestHandler::RequestHandler(
	BookingService& booking_service) : booking_service_(booking_service)
{
}

Response RequestHandler::handle_ping(const Request& request) const {
	Response response;
	response.version(request.version());
	response.result(http::status::ok);
	response.set(http::field::content_type, "application/json");
	json body = {
		{"status", "ok"}
	};
	response.body() = body.dump();
	response.prepare_payload();
	return response;
}

Response RequestHandler::handle_not_found(const Request& request) const {
	Response response;
	response.version(request.version());
	response.result(http::status::not_found);
	response.set(http::field::content_type, "application/json");
	json body = {
		{"status", "not_found"}
	};
	response.body() = body.dump();
	response.prepare_payload();
	return response;
}

Response RequestHandler::handle_not_allowed(const Request& request) const {
	Response response;
	response.version(request.version());
	response.result(http::status::method_not_allowed);
	response.set(http::field::content_type, "application/json");
	json body = {
		{"status", "method_not_allowed"}
	};
	response.body() = body.dump();
	response.prepare_payload();
	return response;
}

Response RequestHandler::handle_bad_request(const Request& request) const {
	Response response;
	response.version(request.version());
	response.result(http::status::bad_request);
	response.set(http::field::content_type, "application/json");
	json body = {
		{"status", "bad_request"}
	};
	response.body() = body.dump();
	response.prepare_payload();
	return response;
}

Response RequestHandler::handle_book(const Request& request) const {
	json request_json;
	try {
		request_json = json::parse(request.body());
	}
	catch (const json::parse_error&)
	{
		return handle_bad_request(request);
	}
	if (!request_json.is_object()) {
		return handle_bad_request(request);
	}
	std::int64_t resource_id;
	std::string start_time;
	std::string end_time;
	if (request_json.contains("resource_id")) {
		if (request_json.at("resource_id").is_number_integer()) {
			resource_id = request_json.at("resource_id").get<std::int64_t>();
		}
		else {
			return handle_bad_request(request);
		}
	}
	else {
		return handle_bad_request(request);
	}
	if (request_json.contains("start_time")) {
		if (request_json.at("start_time").is_string()) {
			start_time = request_json.at("start_time").get<std::string>();
		}
		else {
			return handle_bad_request(request);
		}
	}
	else {
		return handle_bad_request(request);
	}
	if (request_json.contains("end_time")) {
		if (request_json.at("end_time").is_string()) {
			start_time = request_json.at("end_time").get<std::string>();
		}
		else {
			return handle_bad_request(request);
		}
	}
	else {
		return handle_bad_request(request);
	}
	CreateBookingDto dto{ resource_id, start_time, end_time };
	auto booking_result = booking_service_.create_booking(dto);
	if (booking_result.status == CreateBookingStatus::invalid_data) {
		return handle_bad_request(request);
	}
	if (booking_result.status == CreateBookingStatus::conflict) {
		return handle_conflict(request);
	}
	const Booking& booking =
		booking_result.booking.value();
	Response response;
	response.version(request.version());
	response.result(http::status::created);
	response.set(http::field::content_type, "application/json");
	json response_json = {
		{"status", "created"},
		{"id", booking.id},
		{"resource_id", booking.resource_id}
	};
	response.body() = response_json.dump();
	response.prepare_payload();
	return response;
	
}

Response RequestHandler::handle_get_bookings(const Request& request) const {
	Response response;
	response.version(request.version());
	response.result(http::status::ok);
	response.set(http::field::content_type, "application/json");
	std::vector<Booking> bookings = booking_service_.get_all_bookings();
	json bookings_json = json::array();
	for (const auto& booking : bookings) {
		json booking_json = {
			{"id", booking.id},
			{"resource_id", booking.resource_id},
			{
				"start_time_epoch",
				booking.start_time
				.time_since_epoch()
				.count()
			},
			{
				"end_time_epoch",
				booking.end_time
				.time_since_epoch()
				.count()
			}
		};
		bookings_json.push_back(booking_json);
		
	}
	json response_json = {
		{"bookings", bookings_json}
	};
	response.body() = response_json.dump();
	response.prepare_payload();
	return response;
}

Response RequestHandler::handle_delete_bookings(const Request& request) const {
	auto target = request.target();
	auto id_opt = parse_booking_id(target);
	if (!id_opt) {
		return handle_bad_request(request);
	}
	std::int64_t id = id_opt.value();
	if (booking_service_.delete_booking(id)) {
		Response response;
		response.version(request.version());
		response.result(http::status::no_content);
		return response;
	}
	else {
		return handle_not_found(request);
	}
}

Response RequestHandler::handle_conflict(const Request& request) const {
	Response response;
	response.version(request.version());
	response.result(http::status::conflict);
	response.set(http::field::content_type, "application/json");
	json body = {
		{"status", "conflict"}
	};
	response.body() = body.dump();
	response.prepare_payload();
	return response;
}

std::optional<std::int64_t> RequestHandler::parse_booking_id(beast::string_view text) const {
	beast::string_view prefix = "/bookings/";
	auto id_text = text.substr(prefix.size());
	if (id_text.empty()) {
		return std::nullopt;
	}
	std::int64_t id;
	auto result = std::from_chars(
		id_text.data(),
		id_text.data() + id_text.size(),
		id
	);
	if (result.ec == std::errc{} && result.ptr == id_text.data() + id_text.size()) {
		if (id > 0) {
			return id;
		}
		else {
			return std::nullopt;
		}
	}
	else {
		return std::nullopt;
	}
}