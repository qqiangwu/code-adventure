#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "acceptor.h"
#include "net_error.h"

using namespace Net;

namespace {
    /**
     * @throws std::system_error if failed to open
     */
    inline Native_handle_type open_acceptor()
    {
        const auto fd = ::socket(AF_INET, SOCK_STREAM, 0);

        if (fd < 0) {
            assert(errno != EPROTOTYPE);
            assert(errno != EPROTONOSUPPORT);
            assert(errno != EAFNOSUPPORT);

            if (errno == EACCES) {
                throw_net_error<Net_access_error>();
            } else {
                throw_net_error<Resource_not_enough>();
            }
        }

        return fd;
    }
}

Acceptor::Acceptor(const Ipv4_addr addr)
    : File_handle(open_acceptor()), addr_(addr)
{
    int enable_reuse_addr = 1;
    auto r = ::setsockopt(handle(), SOL_SOCKET, SO_REUSEADDR, &enable_reuse_addr, sizeof(enable_reuse_addr));
    if (r < 0) {
        assert(errno != EBADF);
        assert(errno != EFAULT);
        assert(errno != EINVAL);
        assert(errno != ENOPROTOOPT);
        assert(errno != ENOTSOCK);
        assert(errno != EISCONN);

        throw_net_error<Resource_not_enough>();
    }

    sockaddr_in server_addr {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(addr.ip().value());
    server_addr.sin_port = htons(addr.port());

    r = ::bind(handle(), reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
    if (r < 0) {
        assert(errno != EAFNOSUPPORT);
        assert(errno != EBADF);
        assert(errno != EDESTADDRREQ);
        assert(errno != EFAULT);
        assert(errno != ENOTSOCK);
        assert(errno != EOPNOTSUPP);

        switch (errno) {
        case EACCES: throw_net_error<Net_access_error>();
        case EADDRINUSE: throw_net_error<Address_in_use>(addr);
        case EADDRNOTAVAIL: throw_net_error<Address_not_available>(addr);
        default:
            assert(false && "invalid errno");
            break;
        }
    }

    r = ::listen(handle(), 1024);
    if (r < 0) {
        assert(errno != EBADF);
        assert(errno != EDESTADDRREQ);
        assert(errno != EINVAL);
        assert(errno != ENOTSOCK);
        assert(errno != EOPNOTSUPP);

        throw_net_error<Net_access_error>();
    }
}

std::unique_ptr<Socket> Acceptor::accept()
{
    assert(!is_nonblocking());

    for (;;) {
        if (auto p = native_accept_()) {
            return p;
        }
    }
}

std::unique_ptr<Socket> Acceptor::try_accept()
{
    assert(is_nonblocking());

    return native_accept_();
}

std::unique_ptr<Socket> Acceptor::native_accept_()
{
    sockaddr_in remote {};
    socklen_t len = sizeof(remote);

    for (;;) {
        const int fd = ::accept(handle(), reinterpret_cast<sockaddr*>(&remote), &len);
        if (fd >= 0) {
            Ip ip(ntohl(remote.sin_addr.s_addr));
            Ipv4_addr remote_(ip, ntohs(remote.sin_port));

            return std::unique_ptr<Socket>(new Socket(fd, addr_, remote_));
        } else {
            assert(errno != EFAULT);
            assert(errno != EBADF);
            assert(errno != EINVAL);
            assert(errno != ENOTSOCK);
            assert(errno != EOPNOTSUPP);
            assert(errno != ECONNABORTED);

            if (errno == EAGAIN) {
                return nullptr;
            } else if (errno == EINTR) {
                continue;
            }

            // for EMFILE/ENOMEM
            throw_net_error<Resource_not_enough>();

            return nullptr;
        }
    }
}