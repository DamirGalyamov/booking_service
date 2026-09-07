#include "Router.hpp"

Router::Router(RequestHandler& handler): handler_(handler) 
{
}

Response Router::route(const Request& request ) const {
	const beast::string_view booking_prefix = "/bookings/";
	if (request.target() == "/ping") {
		if (request.method() == http::verb::get) {
			return handler_.handle_ping(request);
		}
		else {
			return handler_.handle_not_allowed(request);
		}
	}
	else if (request.target() == "/book") {
		if (request.method() == http::verb::post) {
			return handler_.handle_book(request);
		}
		else {
			return handler_.handle_not_allowed(request);
		}
	}
	else if (request.target() == "/bookings") {
		if (request.method() == http::verb::get) {
			return handler_.handle_get_bookings(request);
		}
		else {
			return handler_.handle_not_allowed(request);
		}
	}
	else if (request.target().size() >= booking_prefix.size() &&
		request.target().substr(0, booking_prefix.size()) == booking_prefix) {
		if (request.method() == http::verb::delete_) {
			return handler_.handle_delete_bookings(request);

		}
		else {
			return handler_.handle_not_allowed(request);
		}
	}
	else {
		return handler_.handle_not_found(request);
	}
}