//
// Created by wuqq on 26/01/2018.
//

#include <cerrno>
#include <algorithm>
#include "selector.h"
#include "acceptor.h"

using namespace Net;

namespace {
    inline timeval to_timeval(const std::chrono::milliseconds timeout) noexcept
    {
        using namespace std::chrono;

        timeval time {};

        const auto sec = duration_cast<seconds>(timeout);
        const auto usec = microseconds(timeout - sec);

        time.tv_sec = sec.count();
        time.tv_usec = usec.count();

        return time;
    }
}

int Selector::io_object_count() const noexcept
{
    return std::max(acceptors_.size() + readers_.size(), writers_.size());
}

void Selector::add_acceptor(Observer_ptr<Acceptor> acceptor, EventHandler<Acceptor> handler) noexcept
{
    acceptors_.emplace(acceptor, handler);
}

void Selector::add_readable_socket(Observer_ptr<Socket> socket, EventHandler<Socket> handler) noexcept
{
    readers_.emplace(socket, handler);
}

void Selector::add_writeable_socket(Observer_ptr<Socket> socket, EventHandler<Socket> handler) noexcept
{
    writers_.emplace(socket, handler);
}

void Selector::remove_acceptor(Observer_ptr<Acceptor> acceptor) noexcept
{
    acceptors_.erase(acceptor);
    FD_CLR(acceptor->handle(), &readable_fds_);
}

void Selector::remove_readable_socket(Observer_ptr<Socket> socket) noexcept
{
    readers_.erase(socket);
    FD_CLR(socket->handle(), &readable_fds_);
}

void Selector::remove_writable_socket(Observer_ptr<Socket> socket) noexcept
{
    writers_.erase(socket);
    FD_CLR(socket->handle(), &writeable_fds_);
}

bool Selector::select(std::chrono::milliseconds timeout)
{
    if (!has_registered_events_()) {
        return false;
    }

    prepare_fd_();

    if (do_select_(timeout)) {
        do_dispatch_();
        return true;
    } else {
        return false;
    }
}

bool Selector::has_registered_events_() const noexcept
{
    return !acceptors_.empty() ||
            !readers_.empty() ||
            !writers_.empty();
}

void Selector::prepare_fd_() noexcept
{
    FD_ZERO(&readable_fds_);
    FD_ZERO(&writeable_fds_);
    max_fd_ = 0;

    // init fds
    for (auto& acceptor: acceptors_) {
        FD_SET(acceptor.first->handle(), &readable_fds_);
        max_fd_ = std::max(max_fd_, acceptor.first->handle());
    }

    for (auto& socket: readers_) {
        FD_SET(socket.first->handle(), &readable_fds_);
        max_fd_ = std::max(max_fd_, socket.first->handle());
    }

    for (auto& socket: writers_) {
        FD_SET(socket.first->handle(), &writeable_fds_);
        max_fd_ = std::max(max_fd_, socket.first->handle());
    }
}

bool Selector::do_select_(std::chrono::milliseconds timeout)
{
    timeval timeout_ = to_timeval(timeout);

    const auto ret = ::select(max_fd_ + 1,
            &readable_fds_,
            &writeable_fds_,
            nullptr,
            timeout == timeout.max()? nullptr: &timeout_);

    if (ret < 0) {
        assert(errno != EBADF);
        assert(errno != EINVAL);

        if (errno != EAGAIN) {
            throw_net_error<Resource_not_enough>();
        }
    }

    return ret > 0;
}

void Selector::do_dispatch_() noexcept
{
    auto current_acceptors = acceptors_;
    auto current_readers = readers_;
    auto current_writers = writers_;

    for (auto& acceptor: current_acceptors) {
        if (FD_ISSET(acceptor.first->handle(), &readable_fds_)) {
            const auto alive = acceptor.second(acceptor.first);

            if (!alive) {
                remove_acceptor(acceptor.first);
            }
        }
    }

    for (auto& reader: current_readers) {
        if (FD_ISSET(reader.first->handle(), &readable_fds_)) {
            const auto alive = reader.second(reader.first);

            if (!alive) {
                remove_readable_socket(reader.first);
            }
        }
    }

    for (auto& writer: current_writers) {
        if (FD_ISSET(writer.first->handle(), &writeable_fds_)) {
            const auto alive = writer.second(writer.first);

            if (!alive) {
                remove_writable_socket(writer.first);
            }
        }
    }
}