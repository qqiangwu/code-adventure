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
         *
         * read until the buffer is full or EOF
         *
         * @return number of bytes read(>= 0), -1 for EOF
         * @throws Connection_reset
         * @throws Operation_timeout
         * @throws Resource_not_enough
         *
         */
        int read(std::vector<char>& buffer) noexcept;

        /**
         * read nonblockingly, works in nonblocking mode.
         * in blocking mode, it might block.
         *
         * @return number of bytes read(>= 0), -1 for EOF
         * @throws Connection_reset
         * @throws Operation_timeout
         * @throws Resource_not_enough
         *
         */
        int read_some(std::vector<char>& buffer) noexcept;

        /**
         *
         * write until buffer is exhausted or connection is closed
         *
         * @return number of bytes written(>= 0)
         * @throws Connection_reset
         * @throws Net_down
         * @throws Net_unreachable
         * @throws Resource_not_enough
         * @throws Remote_closed
         *
         */
        int write(const std::vector<char>& buffer) noexcept;

        /**
         * write nonblockingly, works in nonblocking mode.
         * in blocking mode, it might block.
         *
         * @return number of bytes written(>= 0)
         * @throws Connection_reset
         * @throws Net_down
         * @throws Net_unreachable
         * @throws Resource_not_enough
         * @throws Remote_closed
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
