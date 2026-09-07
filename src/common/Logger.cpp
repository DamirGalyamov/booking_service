#include "Logger.hpp"
#include <iostream>
#include <thread>

void Logger::info(const std::string& message) {
	log("INFO", message);
}
void Logger::error(const std::string& message) {
	log("ERROR", message);
}

void Logger::log(
	const char* level,
	const std::string& message)
{
	std::lock_guard<std::mutex> lock(mutex_);
	std::cerr
		<< '[' << level << "] "
		<< "[thread " << std::this_thread::get_id() << "] "
		<< message
		<< '\n';
}