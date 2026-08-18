cmake_minimum_required(VERSION 3.20)

project(booking_service LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(nlohmann_json CONFIG REQUIRED)

add_executable(booking_service
    src/main.cpp
)

target_link_libraries(booking_service
    PRIVATE
        nlohmann_json::nlohmann_json
)