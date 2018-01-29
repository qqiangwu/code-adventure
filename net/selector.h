//
// Created by wuqq on 26/01/2018.
//

#ifndef EXPR_SELECTOR_H
#define EXPR_SELECTOR_H

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <chrono>
#include <functional>
#include <sys/select.h>
#include "common_fwd.h"
#include "common_util.h"
#include "file_handle.h"
#include "net_error.h"

namespace Net {
    /**
     * @returns false to remove the io object.
     */
    template <class IoObject>
    using EventHandler = std::function<bool(Observer_ptr<IoObject>) noexcept>;

    /**
     * not thread safe
     */
    class Selector : private Noncopyable {
    public:
        static constexpr int max_io_object_supported = FD_SETSIZE;

        int io_object_count() const noexcept;

        /**
         * @param acceptor
         * @param handler
         * @pre   io_object_count() < max_io_object_supported
         * duplicated adding has no effect
         */
        void add_acceptor(Observer_ptr<Acceptor> acceptor, EventHandler<Acceptor> handler) noexcept;

        /**
         * duplicated adding has no effect
         *
         * @param socket
         * @param handler
         * @pre   io_object_count() < max_io_object_supported
         */
        void add_readable_socket(Observer_ptr<Socket> socket, EventHandler<Socket> handler) noexcept;

        /**
         * duplicated adding has no effect
         *
         * @param socket
         * @param handler
         * @pre   io_object_count() < max_io_object_supported
         */
        void add_writeable_socket(Observer_ptr<Socket> socket, EventHandler<Socket> handler) noexcept;

        /**
         * no effect if the acceptor not exist.
         *
         * invalidate all pending events.
         *
         * @param acceptor
         */
        void remove_acceptor(Observer_ptr<Acceptor> acceptor) noexcept;

        /**
         * no effect if the socket not exist
         *
         * invalidate all pending events.
         *
         * @param socket
         */
        void remove_readable_socket(Observer_ptr<Socket> socket) noexcept;

        /**
         * no effect if the socket not exist
         *
         * invalidates all pending events
         *
         */
        void remove_writable_socket(Observer_ptr<Socket> socket) noexcept;

        /**
         * @param timeout  0 for immediately, max for blocking
         * @returns true if events occurs
         *
         * @throws Resource_not_enough
         *
         */
        bool select(std::chrono::milliseconds timeout);

    private:
        bool has_registered_events_() const noexcept;

        void prepare_fd_() noexcept;

        bool do_select_(std::chrono::milliseconds timeout);

        void do_dispatch_() noexcept;

    private:
        std::unordered_map<Observer_ptr<Acceptor>, EventHandler<Acceptor>> acceptors_;
        std::unordered_map<Observer_ptr<Socket>, EventHandler<Socket>> readers_;
        std::unordered_map<Observer_ptr<Socket>, EventHandler<Socket>> writers_;

        fd_set readable_fds_;
        fd_set writeable_fds_;
        int max_fd_;
    };
}

#endif //EXPR_SELECTOR_H
