cmake_minimum_required(VERSION 3.20)

project(booking_service LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(nlohmann_json CONFIG REQUIRED)
find_package(boost_asio CONFIG REQUIRED)
find_package(boost_beast CONFIG REQUIRED)
find_package(libpqxx CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)

add_executable(
    booking_service
    src/main.cpp
    src/http/Server.cpp
    src/http/Router.cpp
    src/http/RequestHandler.cpp
    src/database/Database.cpp
    src/database/ConnectionPool.cpp
    src/booking/BookingService.cpp
    src/booking/BookingRepository.cpp
    src/common/Logger.cpp
    src/config/Config.cpp
)

target_link_libraries(booking_service
    PRIVATE
        nlohmann_json::nlohmann_json
        Boost::asio
        Boost::beast
        libpqxx::pqxx
)

enable_testing()

add_executable(
    booking_unit_tests
    tests/unit/BookingServiceTests.cpp
    src/booking/BookingService.cpp
)

target_link_libraries(
    booking_unit_tests
    PRIVATE
        GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(
    booking_unit_tests
)