#pragma once

#include<string>
#include <mutex>

class Logger
{
public:
	void info(const std::string& message);
	void error(const std::string& message);

private:
	void log(
		const char* level,
		const std::string& message);

	std::mutex mutex_;
};