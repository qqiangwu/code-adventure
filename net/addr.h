#ifndef NET_IPV4_ADDR_H_
#define NET_IPV4_ADDR_H_

#include <cstdint>
#include <string>

namespace Net {
    class Ip {
    public:
        explicit Ip(const int ip) noexcept
            : ip_(ip)
        {
        }

        const uint32_t value() const noexcept
        {
            return ip_;
        }

        std::string str() const noexcept;

    public:
        /**
         * @throws std::invalid_argument if string is not valid
         */
        static Ip from_string(const std::string& ip);

        static Ip any() noexcept
        {
            return Ip(0);
        }

    private:
        std::uint32_t ip_;
    };

    class Ipv4_addr {
    public:
        Ipv4_addr(const Ip ip, const std::uint16_t port) noexcept
            : ip_(ip), port_(port)
        {}

        Ip ip() const noexcept
        {
            return ip_;
        }

        std::uint16_t port() const noexcept
        {
            return port_;
        }

        std::string str() const noexcept
        {
            return ip_.str() + ":" + std::to_string(port_);
        }

    private:
        Ip ip_;
        std::uint16_t port_;
    };
}

#endif
