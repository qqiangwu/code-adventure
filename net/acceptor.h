#ifndef NET_ACCEPTOR_H_
#define NET_ACCEPTOR_H_

#include <memory>
#include "common_util.h"
#include "addr.h"
#include "file_handle.h"
#include "socket.h"

namespace Net {
    /**
     * not thread safe
     */
    class Acceptor : public Detail::File_handle {
    public:
        /**
         * @throws std::system_error if failed to start acceptor
         */
        explicit Acceptor(Ipv4_addr addr);

    public:
        /**
         * @return nullptr if no socket is accepted.
         * @throws std::system_error if io error occurs
         */
        std::unique_ptr<Socket> accept();

    private:
        const Ipv4_addr addr_;
    };
}

#endif
