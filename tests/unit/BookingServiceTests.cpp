#include <gtest/gtest.h>

#include "../../src/booking/BookingService.hpp"
#include "../../src/booking/IBookingRepository.hpp"
#include <optional>
#include <vector>
#include <sstream>

class FakeBookingRepository : public IBookingRepository
{
public:
	std::optional<Booking> create_result;

	int try_create_calls = 0;

	std::optional<Booking> try_create(
		const NewBooking& booking) override
	{
		++try_create_calls;

		return create_result;
	}

	std::vector<Booking> get_all() override
	{
		return {};
	}

	bool remove(std::int64_t id) override
	{
		return false;
	}
};

TEST(
	BookingServiceTest,
	RejectsInvalidResourceId
)
{
	FakeBookingRepository repository;
	BookingService service(repository);

	CreateBookingDto dto{
		0,
		"2026-09-10T10:00:00Z",
		"2026-09-10T12:00:00Z"
	};

	auto result =
		service.create_booking(dto);

	EXPECT_EQ(
		result.status,
		CreateBookingStatus::invalid_data
	);

	EXPECT_EQ(
		repository.try_create_calls,
		0
	);
}
TEST(
	BookingServiceTest,
	RejectsInvalidStartTime
)
{
	FakeBookingRepository repository;
	BookingService service(repository);

	CreateBookingDto dto{
		1,
		"2o26-09-10T10:00:00Z",
		"2026-09-10T12:00:00Z"
	};
	auto result = service.create_booking(dto);
	EXPECT_EQ(
		result.status,
		CreateBookingStatus::invalid_data
	);
	EXPECT_EQ(
		repository.try_create_calls,
		0
	);
}
TEST(
	BookingServiceTest,
	RejectsInvalidEndTime
)
{
	FakeBookingRepository repository;
	BookingService service(repository);

	CreateBookingDto dto{
		1,
		"2026-09-10T10:00:00Z",
		"2o26-09-10T10:00:00Z"
	};
	auto result = service.create_booking(dto);
	EXPECT_EQ(
		result.status,
		CreateBookingStatus::invalid_data
	);
	EXPECT_EQ(
		repository.try_create_calls,
		0
	);
}
TEST(
	BookingServiceTest,
	RejectsEqualStartAndEnd
)
{
	FakeBookingRepository repository;
	BookingService service(repository);

	CreateBookingDto dto{
		1,
		"2026-09-10T10:00:00Z",
		"2026-09-10T10:00:00Z"
	};
	auto result = service.create_booking(dto);
	EXPECT_EQ(
		result.status,
		CreateBookingStatus::invalid_data
	);
	EXPECT_EQ(
		repository.try_create_calls,
		0
	);
}
TEST(
	BookingServiceTest,
	RejectsStartAfterEnd
)
{
	FakeBookingRepository repository;
	BookingService service(repository);

	CreateBookingDto dto{
		1,
		"2026-09-10T10:00:00Z",
		"2026-09-10T09:00:00Z"
	};
	auto result = service.create_booking(dto);
	EXPECT_EQ(
		result.status,
		CreateBookingStatus::invalid_data
	);
	EXPECT_EQ(
		repository.try_create_calls,
		0
	);
}
TEST(
	BookingServiceTest,
	ReturnsConflict
)
{
	FakeBookingRepository repository;
	BookingService service(repository);

	CreateBookingDto dto{
		1,
		"2026-09-10T10:00:00Z",
		"2026-09-10T12:00:00Z"
	};
	repository.create_result = std::nullopt;
	auto result = service.create_booking(dto);
	EXPECT_EQ(
		result.status,
		CreateBookingStatus::conflict
	);
}
TEST(
	BookingServiceTest,
	ReturnsCreated
)
{
	FakeBookingRepository repository;
	BookingService service(repository);

	CreateBookingDto dto{
		1,
		"2026-09-10T10:00:00Z",
		"2026-09-10T12:00:00Z"
	};
	const auto start_time = std::chrono::sys_days{std::chrono::year{2026}/9/10} + std::chrono::hours{10};
const auto end_time = start_time + std::chrono::hours{2};
repository.create_result = Booking{
		1,
		dto.resource_id,
		start_time,
		end_time
	};
	auto result = service.create_booking(dto);
	EXPECT_EQ(
		result.status,
		CreateBookingStatus::created
	);

}
#include "../../src/http/Router.hpp"
TEST(HttpRegression, ValidBookingReachesRepository) {
    FakeBookingRepository repository;
    repository.create_result = Booking{1, 7, {}, {}};
    BookingService service(repository);
    RequestHandler handler(service);
    Router router(handler);
    Request request{http::verb::post, "/book", 11};
    request.body() = R"({"resource_id":7,"start_time":"2026-09-10T10:00:00Z","end_time":"2026-09-10T12:00:00Z"})";
    auto response = router.route(request);
    EXPECT_EQ(response.result(), http::status::created);
    EXPECT_EQ(repository.try_create_calls, 1);
}
TEST(HttpRegression, InvalidJsonIsRejected) {
    FakeBookingRepository repository;
    BookingService service(repository);
    RequestHandler handler(service);
    Request request{http::verb::post, "/book", 11};
    request.body() = "{";
    EXPECT_EQ(handler.handle_book(request).result(), http::status::bad_request);
    EXPECT_EQ(repository.try_create_calls, 0);
}
