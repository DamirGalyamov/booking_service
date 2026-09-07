BOOKING Service
Многопоточный HTTP книжный сервис написанный на C++20.
Сервис позволяет клиенту создать, получить список и удалить запись о книге. Данные о книгах расположены в PostgresSQL.
Проект создавался как практика в создании бэкенд проектов на языке С++.
Архитектура
Приложение можно разбить на несколько уровней:
   HTTP Client
	|
	v
     Server
	|
	v
     Router
	|
	v
  RequestHandler
	|
	v
  BookingService
	|
	v
 IBookingRepository
	|
	v
 BookingRepository
	|
	v
  ConnectionPool
	|
	v
   PostgresSQL
API
Ping
GET/ ping

Response:
{
 "status": "ok"
}

Create booking
POST/ book
Content-Type: application/json

Example request
{
  "resource_id": 1,
  "start_time": "2026-09-10T10:00:00Z",
  "end_time": "2026-09-10T12:00:00Z"
}

Possible responses:
201 Created — booking created
400 Bad Request — invalid input
409 Conflict — overlapping booking already exists
500 Internal Server Error — unexpected server failure

List bookings
GET /bookings



Delete booking
DELETE /bookings/{id}

Possible responses:
204 No Content — booking deleted
400 Bad Request — invalid booking ID
404 Not Found — booking does not exist

Requirements

C++20 compiler
CMake 3.20+
vcpkg
Docker / Docker Compose
PostgreSQL

Main libraries:
Boost.Asio
Boost.Beast
libpqxx
nlohmann/json
GoogleTest

Configuration
The application reads its configuration from environment variables:

BOOKING_DB_CONNECTION
BOOKING_PORT
BOOKING_WORKERS
BOOKING_POOL_SIZE



Example for PowerShell:

$env:BOOKING_DB_CONNECTION = "host=127.0.0.1 port=5433 dbname=booking user=booking password=booking_dev"
$env:BOOKING_PORT = "8080"
$env:BOOKING_WORKERS = "4"
$env:BOOKING_POOL_SIZE = "4"

Build
Configure the project:
cmake --preset local-debug

Build:
cmake --build build/local-debug --config Debug

Run:
.\build\local-debug\Debug\booking_service.exe
PostgreSQL

Start PostgreSQL:
docker compose up -d
The local development setup exposes PostgreSQL on port 5433.

Tests
The project contains unit tests for BookingService.
The tests use a fake implementation of IBookingRepository, so PostgreSQL is not required for unit testing.

Run tests:
ctest --test-dir build/local-debug -C Debug --output-on-failure