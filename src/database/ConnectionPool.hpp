#pragma once

#include "Database.hpp"

#include<pqxx/pqxx>
#include<vector>
#include<queue>
#include <mutex>
#include <condition_variable>
#include <cstddef>

class ConnectionPool {
public:

	class Lease {
	public:
		Lease(ConnectionPool& pool, std::size_t index);
		Lease(const Lease&) = delete;
		Lease& operator=(const Lease&) = delete;
		~Lease();
		pqxx::connection& connection();
	private:
		ConnectionPool& pool_;
		std::size_t index_;
	};
	ConnectionPool( Database & database, std::size_t pool_size );

	Lease acquire();

private:
	void release(std::size_t index);

	std::mutex mutex_;
	std::condition_variable cv_;
	std::queue<std::size_t> available_;
	std::vector<pqxx::connection> connections_;
};