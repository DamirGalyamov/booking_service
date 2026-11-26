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

Ответ:
{
 "status": "ok"
}

Создание брони
POST/ book
Content-Type: application/json

Пример запроса
{
  "resource_id": 1,
  "start_time": "2026-09-10T10:00:00Z",
  "end_time": "2026-09-10T12:00:00Z"
}

Возможные ответы:
201 Created — booking created
400 Bad Request — invalid input
409 Conflict — overlapping booking already exists
500 Internal Server Error — unexpected server failure

Список бронирования
GET /bookings



Удаление брони
DELETE /bookings/{id}

Возможные ответы:
204 No Content — booking deleted
400 Bad Request — invalid booking ID
404 Not Found — booking does not exist

Требования

C++20 compiler
CMake 3.20+
vcpkg
Docker / Docker Compose
PostgreSQL

Основные библиотеки:
Boost.Asio
Boost.Beast
libpqxx
nlohmann/json
GoogleTest

Конфигурация
Приложение считывает свою конфигурацию из переменных окружения:

BOOKING_DB_CONNECTION
BOOKING_PORT
BOOKING_WORKERS
BOOKING_POOL_SIZE



Пример для PowerShell:

$env:BOOKING_DB_CONNECTION = "host=127.0.0.1 port=5433 dbname=booking user=booking password=booking_dev"
$env:BOOKING_PORT = "8080"
$env:BOOKING_WORKERS = "4"
$env:BOOKING_POOL_SIZE = "4"

Сборка
Configure the project:
cmake --preset local-debug

Сборка:
cmake --build build/local-debug --config Debug

Запуск:
.\build\local-debug\Debug\booking_service.exe
PostgreSQL

Старт PostgreSQL:
docker compose up -d
В среде локальной разработки PostgreSQL доступен через порт 5433.

Тесты
В проекте содержатся модульные тесты для BookingService.
В тестах используется фиктивная реализация IBookingRepository, поэтому для модульного тестирования PostgreSQL не требуется.

Запуск тестов:
ctest --test-dir build/local-debug -C Debug --output-on-failure
