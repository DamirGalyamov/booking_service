#include "ConnectionPool.hpp"

ConnectionPool::ConnectionPool(Database& database, std::size_t pool_size) {
	connections_.reserve(pool_size);
	for (std::size_t i = 0; i < pool_size; i++) {
		connections_.push_back(database.create_connection());
		available_.push(i);
	}
}

ConnectionPool::Lease ConnectionPool::acquire() {
	std::unique_lock<std::mutex> lock(mutex_);
	cv_.wait(lock, [this] {
		return !available_.empty();
		}

	);
	std::size_t index = available_.front();
	available_.pop();
	return Lease{ *this, index };
	
}

void ConnectionPool::release(std::size_t index) {
	{
		std::lock_guard<std::mutex> lock(mutex_);
		available_.push(index);
	}
	cv_.notify_one();
}

ConnectionPool::Lease::Lease(ConnectionPool& pool, std::size_t index):
	pool_(pool), index_(index)
{}

ConnectionPool::Lease::~Lease() {
	pool_.release(index_);
}
pqxx::connection& ConnectionPool::Lease::connection() {
	return pool_.connections_[index_];
}