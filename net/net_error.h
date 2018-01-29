//
// Created by wuqq on 30/01/2018.
//

#ifndef EXPR_NET_ERROR_H
#define EXPR_NET_ERROR_H

#include <system_error>
#include <cerrno>
#include <type_traits>
#include "addr.h"

namespace Net {
    class Net_error : public std::system_error {
    public:
        Net_error()
            : system_error(std::error_code(errno, std::system_category()))
        {}
    };

    class Net_access_error : public Net_error {
    public:
        using Net_error::Net_error;
    };

    class Address_in_use : public Net_error {
    public:
        Address_in_use(Ipv4_addr addr) : addr_(addr)
        {}

        Ipv4_addr addr() const noexcept
        {
            return addr_;
        }

    private:
        const Ipv4_addr addr_;
    };

    class Address_not_available : public Net_error {
    public:
        Address_not_available(Ipv4_addr addr) : addr_(addr)
        {}

        Ipv4_addr addr() const noexcept
        {
            return addr_;
        }

    private:
        const Ipv4_addr addr_;
    };

    class Resource_not_enough : public Net_error {
    public:
        using Net_error::Net_error;
    };

    class Connection_reset : public Net_error {
    public:
        using Net_error::Net_error;
    };

    class Net_down : public Net_error {
    public:
        using Net_error::Net_error;
    };

    class Net_unreachable : public Net_error {
    public:
        Net_unreachable(Ipv4_addr target)
            : target_(target)
        {}

        Ipv4_addr target() const noexcept
        {
            return target_;
        }

    private:
        const Ipv4_addr target_;
    };

    class Remote_closed : public Net_error {
    public:
        using Net_error::Net_error;
    };

    template <class Error, class... Args>
    inline void throw_net_error(Args... args)
    {
        throw Error(std::forward<Args>(args)...);
    }
}

#endif //EXPR_NET_ERROR_H
