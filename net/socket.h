#ifndef NET_SOCKET_H_
#define NET_SOCKET_H_

#include <cassert>
#include <vector>
#include "addr.h"

namespace Net {
    class Connector;
    class Acceptor;
    class Selector;

    class Socket {
    public:
        ~Socket();

        Socket(const Socket&) = delete;
        Socket(Socket&&) = delete;

        Socket& operator=(const Socket&) = delete;
        Socket& operator=(Socket&&) = delete;

    public:
        Ipv4_addr local_addr() const noexcept
        {
            return local_;
        }

        Ipv4_addr remote_addr() const noexcept
        {
            return remote_;
        }

    public:
        /**
         * \return as the same as syscall
         */
        int read(std::vector<char>& buffer) noexcept;

        /**
         * \return as the same as syscall
         */
        int write(const std::vector<char>& buffer) noexcept;

    private:
        friend class Connector;
        friend class Acceptor;
        friend class Selector;

    private:
        /**
         * \pre fd is valid
         */
        Socket(int fd, Ipv4_addr local, Ipv4_addr remote)
            : fd_(fd), local_(local), remote_(remote)
        {
            assert(fd >= 0);
        }

        int fd_;
        Ipv4_addr local_;
        Ipv4_addr remote_;
    };
}

#endif
