#include "booking/BookingRepository.hpp"
#include "booking/BookingService.hpp"
#include "http/RequestHandler.hpp"
#include "http/Router.hpp"
#include "http/Server.hpp"
#include "database/Database.hpp"
#include "database/ConnectionPool.hpp"
#include "common/Logger.hpp"
#include "config/Config.hpp"

#include <boost/asio.hpp>


namespace beast = boost::beast;
namespace http = beast::http;
using tcp = boost::asio::ip::tcp;



int main()
{
    Logger logger;
    try {
        Config config = load_config();
        Database database(config.db_connection_string);
        ConnectionPool pool(database, config.pool_size);
        BookingRepository booking_repository(pool);
        BookingService booking_service(booking_repository);
        RequestHandler request_handler(booking_service);
        Router router(request_handler);
        boost::asio::io_context io_context;
        auto address = boost::asio::ip::make_address("0.0.0.0");
        tcp::endpoint endpoint(address, config.port);
        Server server(io_context, endpoint, router, logger);
        logger.info(
            "starting booking server on port " +
            std::to_string(config.port)
        );
        server.run(config.worker_count);
    }
    catch (const std::exception& error) {
        logger.error(
            "Fatal startup error: " +
            std::string{error.what()}
        );
        return 1;
    }
    return 0;
}