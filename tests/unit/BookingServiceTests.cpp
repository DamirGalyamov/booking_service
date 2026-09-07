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
	std::chrono::sys_seconds start_time;
	{
		std::istringstream stream(dto.start_time);
		std::chrono::from_stream(
			stream,
			"%Y-%m-%dT%H:%M:%SZ",
			start_time);
	}
	std::chrono::sys_seconds end_time;
	{
		std::istringstream stream(dto.end_time);
		std::chrono::from_stream(
			stream,
			"%Y-%m-%dT%H:%M:%SZ",
			end_time);
	}
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