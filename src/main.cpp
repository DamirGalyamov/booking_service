#include <iostream>
#include <nlohmann/json.hpp>

int main()
{
    nlohmann::json message = {
        {"service", "booking-service"},
        {"status", "started"}
    };

    std::cout << message.dump() << '\n';

    return 0;
}