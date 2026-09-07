#include "BookingRepository.hpp"

BookingRepository::BookingRepository(ConnectionPool& pool) :
	pool_(pool) 
{
}

std::optional<Booking> BookingRepository::try_create(const NewBooking& booking) {
	auto start_in_seconds =
		booking.start_time.time_since_epoch().count();
	auto end_in_seconds =
		booking.end_time.time_since_epoch().count();

	auto leas = pool_.acquire();
	pqxx::work transaction(leas.connection());
	transaction.exec(
		R"(
			SELECT pg_advisory_xact_lock($1::bigint);
		)",
		pqxx::params{
				booking.resource_id,
		}
	);
	pqxx::result result_of_overlaping = transaction.exec(
		R"(
			SELECT EXISTS(
				SELECT 1
				FROM bookings
				WHERE resource_id = $1
					AND start_time <
						to_timestamp($3::double precision)
					AND end_time >
						to_timestamp($2::double precision)
			) AS has_overlap;
		)",
		pqxx::params{
				booking.resource_id,
				start_in_seconds,
				end_in_seconds
		}
	);
	pqxx::row overlap_row = result_of_overlaping.one_row();
	if (overlap_row["has_overlap"].as<bool>()) {
		transaction.commit();
		return std::nullopt;
	}
	pqxx::result result = transaction.exec(
		R"(
				INSERT INTO bookings(
					resource_id,
					start_time,
					end_time
				)
				VALUES ($1,
						to_timestamp($2::double precision),
						to_timestamp($3::double precision)
						)
				RETURNING id
			)",
		pqxx::params{
			booking.resource_id,
			start_in_seconds,
			end_in_seconds
		}
	);
	pqxx::row row = result.one_row();
	std::int64_t id =
		row[0].as<std::int64_t>();
	transaction.commit();
	Booking created_booking{ id, booking.resource_id,
	booking.start_time, booking.end_time };
	return created_booking;
}

std::vector<Booking> BookingRepository::get_all() {
	auto leas = pool_.acquire();
	pqxx::read_transaction transaction(leas.connection());
	pqxx::result result = transaction.exec(
		R"(
			SELECT
				id,
				resource_id,
				EXTRACT(EPOCH FROM start_time)::BIGINT
					AS start_seconds,
				EXTRACT(EPOCH FROM end_time)::BIGINT
					AS end_seconds
			FROM bookings
			ORDER BY id ASC;
		)"
	);
	std::vector<Booking> bookings;
	for (const auto& row : result) {
		std::int64_t start_seconds =
			row["start_seconds"].as<std::int64_t>();
		std::int64_t end_seconds =
			row["end_seconds"].as<std::int64_t>();
		std::chrono::sys_seconds start_time{
			std::chrono::seconds{
				start_seconds
			}
		};
		std::chrono::sys_seconds end_time{
			std::chrono::seconds{
				end_seconds
			}
		};
		bookings.push_back(
			Booking{
				row["id"].as<std::int64_t>(),
				row["resource_id"].as<std::int64_t>(),
				start_time,
				end_time
			}
		);
	}
	transaction.commit();
	return bookings;
}

bool BookingRepository::remove(std::int64_t id) {
	auto leas = pool_.acquire();
	pqxx::work transaction(leas.connection());
	pqxx::result result = transaction.exec(
		R"(
			DELETE FROM bookings
			WHERE id = $1;
		)",
		pqxx::params{
			id
		}
	);
	bool removed = result.affected_rows() == 1;
	transaction.commit();
	return removed;
}