#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "acceptor.h"
#include "error_util.h"

using namespace Net;

namespace {
    /**
     * @throws std::system_error if failed to open
     */
    inline Native_handle_type open_acceptor()
    {
        const auto fd = ::socket(AF_INET, SOCK_STREAM, 0);

        if (fd < 0) {
            throw_system_error();
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
        throw_system_error();
    }

    sockaddr_in server_addr {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(addr.ip().value());
    server_addr.sin_port = htons(addr.port());

    r = ::bind(handle(), reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
    if (r < 0) {
        throw_system_error();
    }

    r = ::listen(handle(), 1024);
    if (r < 0) {
        throw_system_error();
    }
}

std::unique_ptr<Socket> Acceptor::accept()
{
    sockaddr_in remote {};
    socklen_t len = sizeof(remote);

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

        if (errno == EAGAIN || errno == EINTR) {
            return nullptr;
        }

        // for EMFILE/ENOMEM
        throw_system_error();

        return nullptr;
    }
}
