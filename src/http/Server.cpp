#include "Server.hpp"
#include <iostream>

Server::Server(boost::asio::io_context& io_context,
	const tcp::endpoint& endpoint, Router& router, Logger& logger): io_context_(io_context),
	acceptor_(io_context, endpoint), router_(router), logger_(logger)
{}

void Server::handle_connection(tcp::socket& socket) {
	beast::flat_buffer buffer;
	Request request;
	boost::system::error_code ec;
	http::read(socket, buffer, request, ec);
	if (ec) {
		logger_.error(
			"HTTP read error: " + ec.message()
		);
		return;
	}
	Response response;
	try {
		response = router_.route(request);
	}
	catch (const std::exception& error) {
		logger_.error(
			"Request processing failed: "+
			std::string{error.what()}
		);

		response = make_internal_error_response(request);
	}
	response.keep_alive(false);
	http::write(socket, response, ec);
	if (ec) {
		logger_.error(
			"HTTP write error: " + ec.message()
		);
	}
}

void Server::worker_loop() {
	while (true) {
		tcp::socket socket(io_context_);
		boost::system::error_code ec;
		acceptor_.accept(socket, ec);
		if (ec) {
			logger_.error(
				"Accept error: " + ec.message()
			);

			continue;
		}
		try {
			handle_connection(socket);
		}
		catch (const std::exception& error) {
			logger_.error(
				"Unhandled connection error: " +
				std::string{error.what()}
			);
		}
	}
}

void Server::run(std::size_t worker_count) {
	std::vector<std::jthread> workers;
	for (std::size_t i = 0; i < worker_count; i++) {
		workers.emplace_back(
			[this] {
				worker_loop();
			}
		);
	}
}

Response Server::make_internal_error_response(const Request& request) const {
	Response response;
	response.version(request.version());
	response.result(
		http::status::internal_server_error
	);

	response.set(
		http::field::content_type,
		"application/json"
	);

	response.body() =
		R"({"status":"internal_error"})";
	response.prepare_payload();
	return response;
}