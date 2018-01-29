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
         * @throws Net_access_error if no permissions
         * @throws Address_in_use
         * @throws Address_not_available
         * @throws Resource_not_enough
         */
        explicit Acceptor(Ipv4_addr addr);

    public:
        /**
         * @return nullptr if no socket is accepted.
         * @throws Resource_not_enough
         *
         */
        std::unique_ptr<Socket> accept();

    private:
        const Ipv4_addr addr_;
    };
}

#endif
