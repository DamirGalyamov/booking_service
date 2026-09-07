#pragma once

#include "HttpTypes.hpp"
#include "RequestHandler.hpp"

class Router
{
public:
	explicit Router(RequestHandler& handler);

	Response route(const Request& request) const;

private:
	RequestHandler& handler_;
};