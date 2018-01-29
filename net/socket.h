#ifndef NET_SOCKET_H_
#define NET_SOCKET_H_

#include <cassert>
#include <vector>
#include <deque>
#include "common_fwd.h"
#include "addr.h"
#include "file_handle.h"

namespace Net {
    /**
     * not thread safe
     */
    class Socket : public Detail::File_handle {
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
         * read as many as possible unless -1
         *
         * @return as the same as syscall
         *
         */
        int read(std::vector<char>& buffer) noexcept;

        /**
         * read nonblockingly, works in nonblocking mode.
         * in blocking mode, it might block.
         *
         * @return as the same as syscall
         *
         */
        int read_some(std::vector<char>& buffer) noexcept;

        /**
         * write as many as possible unless -1
         *
         * @return as the same as syscall
         *
         */
        int write(const std::vector<char>& buffer) noexcept;

        /**
         * write nonblockingly, works in nonblocking mode.
         * in blocking mode, it might block.
         *
         * @return as the same as syscall
         *
         */
        int write_some(const std::vector<char>& buffer) noexcept;

    private:
        friend class Connector;
        friend class Acceptor;
        friend class Selector;

    private:
        /**
         * @pre fd is valid
         */
        Socket(int fd, Ipv4_addr local, Ipv4_addr remote)
            : File_handle(fd), local_(local), remote_(remote)
        {
        }

        Ipv4_addr local_;
        Ipv4_addr remote_;
    };
}

#endif
