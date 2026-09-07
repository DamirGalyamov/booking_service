#pragma once

#include "HttpTypes.hpp"
#include "Router.hpp"
#include "../common/Logger.hpp"
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <thread>
#include <vector>

using tcp = boost::asio::ip::tcp;

class Server
{
public:
	Server(boost::asio::io_context& io_context,
		const tcp::endpoint& endpoint, Router& router,
		Logger& logger);

	void run_once();

	void run(std::size_t worker_count);

private:

	void handle_connection(tcp::socket& socket);

	void worker_loop();

	Response make_internal_error_response(const Request& request) const;

	boost::asio::io_context& io_context_;
	tcp::acceptor acceptor_;
	Router& router_;
	Logger& logger_;
};