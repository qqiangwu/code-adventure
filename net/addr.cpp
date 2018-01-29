#include <string>
#include <limits>
#include <sstream>
#include <stdexcept>
#include "addr.h"

using namespace Net;

std::string Ip::str() const noexcept
{
    const auto uint16_max = std::numeric_limits<std::uint16_t>::max();
    std::uint32_t ip = value();
    std::uint8_t d = ip % uint16_max;
    ip /= uint16_max;
    std::uint8_t c = ip % uint16_max;
    ip /= uint16_max;
    std::uint8_t b = ip % uint16_max;
    ip /= uint16_max;
    std::uint8_t a = ip;

    std::ostringstream oss;
    oss << int(a) << "." << int(b) << "." << int(c) << "." << int(d);

    return oss.str();
}

Ip Ip::from_string(const std::string& ip)
{
    std::istringstream iss(ip);
    std::uint8_t a;
    std::uint8_t b;
    std::uint8_t c;
    std::uint8_t d;
    char dot1;
    char dot2;
    char dot3;

    iss >> a >> dot1 >> b >> dot2 >> c >> dot3 >> d;
    if (!iss || dot1 != '.' || dot2 != '.' || dot3 != '.') {
        throw std::invalid_argument("bad ip");
    }

    std::uint32_t value = a;

    value = (value << 8) + b;
    value = (value << 8) + c;
    value = (value << 8) + d;

    return Ip(value);
}
